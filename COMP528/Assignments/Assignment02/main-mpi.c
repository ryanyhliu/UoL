#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include <mpi.h>
#include <stddef.h>

typedef struct
{
    int *tour;
    double totalDistance;
} TourResult;

int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
double **createDistanceMatrix(double **coords, int numOfCoords);
double sqrt(double arg);

int *findCheapestTour(double **dMatrix, int numOfCoords);
int *findFarthestTour(double **dMatrix, int numOfCoords);
int *findNearestTour(double **dMatrix, int numOfCoords);
TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);




int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc < 5) {
        if (world_rank == 0) {
            fprintf(stderr, "Usage: %s <input file> <output file 1> <output file 2> <output file 3>\n", argv[0]);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    printf("---TEST 01---\n");

    char filename[500];
    char outFileName_Cheapest[500];
    char outFileName_Farthest[500];
    char outFileName_Nearest[500];

    strcpy(filename, argv[1]);
    strcpy(outFileName_Cheapest, argv[2]);
    strcpy(outFileName_Farthest, argv[3]);
    strcpy(outFileName_Nearest, argv[4]);

    int numOfCoords = readNumOfCoords(filename);
    double **coords = readCoords(filename, numOfCoords);
    double **dMatrix = createDistanceMatrix(coords, numOfCoords);

    int tourLength = numOfCoords + 1; // Assuming each tour includes all coordinates plus the start/end point

    printf("---TEST 02---\n");

    // Define MPI data type for TourResult
    MPI_Datatype MPI_TOURRESULT;
    MPI_Datatype type[2] = {MPI_INT, MPI_DOUBLE};
    int blocklen[2] = {tourLength, 1};
    MPI_Aint disp[2];
    disp[0] = offsetof(TourResult, tour);
    disp[1] = offsetof(TourResult, totalDistance);
    MPI_Type_create_struct(2, blocklen, disp, type, &MPI_TOURRESULT);
    MPI_Type_commit(&MPI_TOURRESULT);

    printf("---TEST 03---\n");


    // TourResult local_best_tour_cheapest, local_best_tour_farthest, local_best_tour_nearest;
    TourResult local_best_tour_cheapest = { .tour = NULL, .totalDistance = __DBL_MAX__ };
    TourResult local_best_tour_farthest = { .tour = NULL, .totalDistance = __DBL_MAX__ };
    TourResult local_best_tour_nearest = { .tour = NULL, .totalDistance = __DBL_MAX__ };

    local_best_tour_cheapest.tour = (int *)malloc(tourLength * sizeof(int));
    local_best_tour_farthest.tour = (int *)malloc(tourLength * sizeof(int));
    local_best_tour_nearest.tour = (int *)malloc(tourLength * sizeof(int));

    local_best_tour_cheapest.totalDistance = local_best_tour_farthest.totalDistance = local_best_tour_nearest.totalDistance = __DBL_MAX__;

    for (int i = 0; i < numOfCoords; i++) {
        TourResult current_tour_cheapest = cheapestInsertion(dMatrix, numOfCoords, i);
        if (current_tour_cheapest.totalDistance < local_best_tour_cheapest.totalDistance) {
            memcpy(local_best_tour_cheapest.tour, current_tour_cheapest.tour, tourLength * sizeof(int));
            local_best_tour_cheapest.totalDistance = current_tour_cheapest.totalDistance;
        }
        free(current_tour_cheapest.tour);

        TourResult current_tour_farthest = farthestInsertion(dMatrix, numOfCoords, i);
        if (current_tour_farthest.totalDistance < local_best_tour_farthest.totalDistance) {
            memcpy(local_best_tour_farthest.tour, current_tour_farthest.tour, tourLength * sizeof(int));
            local_best_tour_farthest.totalDistance = current_tour_farthest.totalDistance;
        }
        free(current_tour_farthest.tour);

        TourResult current_tour_nearest = nearestAddition(dMatrix, numOfCoords, i);
        if (current_tour_nearest.totalDistance < local_best_tour_nearest.totalDistance) {
            memccpy(local_best_tour_nearest.tour, current_tour_nearest.tour, tourLength * sizeof(int));
            local_best_tour_nearest.totalDistance = current_tour_nearest.totalDistance;
        }
        free(current_tour_nearest.tour);
    }

    printf("---TEST 04---\n");


    // Gather results to the root process
    TourResult *all_tours_cheapest = NULL, *all_tours_farthest = NULL, *all_tours_nearest = NULL;
    if (world_rank == 0) {
        all_tours_cheapest = (TourResult *)malloc(world_size * sizeof(TourResult));
        all_tours_farthest = (TourResult *)malloc(world_size * sizeof(TourResult));
        all_tours_nearest = (TourResult *)malloc(world_size * sizeof(TourResult));

        for (int i = 0; i < world_size; i++) {
            all_tours_cheapest[i].tour = (int *)malloc(tourLength * sizeof(int));
            all_tours_farthest[i].tour = (int *)malloc(tourLength * sizeof(int));
            all_tours_nearest[i].tour = (int *)malloc(tourLength * sizeof(int));
        }
    }

    printf("---TEST 05---\n");


    MPI_Gather(&local_best_tour_cheapest, 1, MPI_TOURRESULT, all_tours_cheapest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_best_tour_farthest, 1, MPI_TOURRESULT, all_tours_farthest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_best_tour_nearest, 1, MPI_TOURRESULT, all_tours_nearest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);

    printf("---TEST 06---\n");


    // Process results in the root process
	if (world_rank == 0) {
		int minIndexCheapest = 0, minIndexFarthest = 0, minIndexNearest = 0;
		double minDistanceCheapest = all_tours_cheapest[0].totalDistance;
		double minDistanceFarthest = all_tours_farthest[0].totalDistance;
		double minDistanceNearest = all_tours_nearest[0].totalDistance;

		for (int i = 1; i < world_size; i++) {
			if (all_tours_cheapest[i].totalDistance < minDistanceCheapest) {
				minDistanceCheapest = all_tours_cheapest[i].totalDistance;
				minIndexCheapest = i;
			}
			if (all_tours_farthest[i].totalDistance < minDistanceFarthest) {
				minDistanceFarthest = all_tours_farthest[i].totalDistance;
				minIndexFarthest = i;
			}
			if (all_tours_nearest[i].totalDistance < minDistanceNearest) {
				minDistanceNearest = all_tours_nearest[i].totalDistance;
				minIndexNearest = i;
			}
		}

        printf("---TEST 07---\n");


		// Write the best tours to their respective files
		writeTourToFile(all_tours_cheapest[minIndexCheapest].tour, numOfCoords + 1, outFileName_Cheapest);
		writeTourToFile(all_tours_farthest[minIndexFarthest].tour, numOfCoords + 1, outFileName_Farthest);
		writeTourToFile(all_tours_nearest[minIndexNearest].tour, numOfCoords + 1, outFileName_Nearest);
	}

    printf("---TEST 08---\n");

    // Clean up
    MPI_Type_free(&MPI_TOURRESULT);
    if (local_best_tour_cheapest.tour != NULL)
    {
        free(local_best_tour_cheapest.tour);
    }
    if (local_best_tour_farthest.tour != NULL)
    {
        free(local_best_tour_farthest.tour);
    }
    if (local_best_tour_nearest.tour != NULL)
    {
        free(local_best_tour_nearest.tour);
    }
        

    printf("---TEST 09---\n");

    if (world_rank == 0) {
        for (int i = 0; i < world_size; i++) {
            // 释放每个结构体中的tour数组
            if (all_tours_cheapest[i].tour != NULL) {
                free(all_tours_cheapest[i].tour);
                all_tours_cheapest[i].tour = NULL;
            }
            if (all_tours_farthest[i].tour != NULL) {
                free(all_tours_farthest[i].tour);
                all_tours_farthest[i].tour = NULL;
            }
            if (all_tours_nearest[i].tour != NULL) {
                free(all_tours_nearest[i].tour);
                all_tours_nearest[i].tour = NULL;
            }
        }
        // 释放结构体数组
        if (all_tours_cheapest != NULL)
        {
            free(all_tours_cheapest);
        }
        if (all_tours_farthest != NULL)
        {
            free(all_tours_farthest);
        }
        if (all_tours_nearest != NULL)
        {
            free(all_tours_nearest);
        }
        
    }


    printf("---TEST 10---\n");

    for (int i = 0; i < numOfCoords; i++) {
        free(coords[i]);
        free(dMatrix[i]);
    }
    free(coords);
    free(dMatrix);

    printf("---TEST 11---\n");


    MPI_Finalize();
    return 0;
}





double **createDistanceMatrix(double **coords, int numOfCoords)
{
	int i, j;

	double **dMatrix = (double **)malloc(numOfCoords * sizeof(double));

	for (i = 0; i < numOfCoords; i++)
	{
		dMatrix[i] = (double *)malloc(numOfCoords * sizeof(double));
	}

#pragma omp parallel for collapse(2)
	for (i = 0; i < numOfCoords; i++)
	{
		for (j = 0; j < numOfCoords; j++)
		{
			double diffX = coords[i][0] - coords[j][0];
			double diffY = coords[i][1] - coords[j][1];
			dMatrix[i][j] = sqrt((diffX * diffX) + (diffY * diffY));
		}
	}

	return dMatrix;
}

