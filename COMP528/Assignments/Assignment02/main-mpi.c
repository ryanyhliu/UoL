#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include <mpi.h>

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

    char filename[500];
    strcpy(filename, argv[1]);
    char outFileName_Cheapest[500];
    strcpy(outFileName_Cheapest, argv[2]);
    char outFileName_Farthest[500];
    strcpy(outFileName_Farthest, argv[3]);
    char outFileName_Nearest[500];
    strcpy(outFileName_Nearest, argv[4]);

    int numOfCoords = readNumOfCoords(filename);
    double **coords = readCoords(filename, numOfCoords);
    double **dMatrix = createDistanceMatrix(coords, numOfCoords);

    int tourLength = numOfCoords + 1; // Assuming each tour includes all coordinates plus the start/end point

    // Define MPI data type for TourResult
    MPI_Datatype MPI_TOURRESULT;
    MPI_Datatype type[2] = {MPI_INT, MPI_DOUBLE};
    int blocklen[2] = {tourLength, 1};
    MPI_Aint disp[2];
    disp[0] = offsetof(TourResult, tour);
    disp[1] = offsetof(TourResult, totalDistance);
    MPI_Type_create_struct(2, blocklen, disp, type, &MPI_TOURRESULT);
    MPI_Type_commit(&MPI_TOURRESULT);

    TourResult local_best_tour_cheapest, local_best_tour_farthest, local_best_tour_nearest;
    local_best_tour_cheapest.tour = (int *)malloc(tourLength * sizeof(int));
    local_best_tour_farthest.tour = (int *)malloc(tourLength * sizeof(int));
    local_best_tour_nearest.tour = (int *)malloc(tourLength * sizeof(int));
    local_best_tour_cheapest.totalDistance = local_best_tour_farthest.totalDistance = local_best_tour_nearest.totalDistance = __DBL_MAX__;

    for (int i = 0; i < numOfCoords; i++) {
        TourResult current_tour_cheapest = cheapestInsertion(dMatrix, numOfCoords, i);
        if (current_tour_cheapest.totalDistance < local_best_tour_cheapest.totalDistance) {
            local_best_tour_cheapest = current_tour_cheapest;
        }

        TourResult current_tour_farthest = farthestInsertion(dMatrix, numOfCoords, i);
        if (current_tour_farthest.totalDistance < local_best_tour_farthest.totalDistance) {
            local_best_tour_farthest = current_tour_farthest;
        }

        TourResult current_tour_nearest = nearestAddition(dMatrix, numOfCoords, i);
        if (current_tour_nearest.totalDistance < local_best_tour_nearest.totalDistance) {
            local_best_tour_nearest = current_tour_nearest;
        }
    }

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

    MPI_Gather(&local_best_tour_cheapest, 1, MPI_TOURRESULT, all_tours_cheapest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_best_tour_farthest, 1, MPI_TOURRESULT, all_tours_farthest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_best_tour_nearest, 1, MPI_TOURRESULT, all_tours_nearest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);

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

		// Write the best tours to their respective files
		writeTourToFile(all_tours_cheapest[minIndexCheapest].tour, numOfCoords + 1, outFileName_Cheapest);
		writeTourToFile(all_tours_farthest[minIndexFarthest].tour, numOfCoords + 1, outFileName_Farthest);
		writeTourToFile(all_tours_nearest[minIndexNearest].tour, numOfCoords + 1, outFileName_Nearest);
	}


    // Clean up
    MPI_Type_free(&MPI_TOURRESULT);
    free(local_best_tour_cheapest.tour);
    free(local_best_tour_farthest.tour);
    free(local_best_tour_nearest.tour);
    if (world_rank == 0) {
        for (int i = 0; i < world_size; i++) {
            free(all_tours_cheapest[i].tour);
            free(all_tours_farthest[i].tour);
            free(all_tours_nearest[i].tour);
        }
        free(all_tours_cheapest);
        free(all_tours_farthest);
        free(all_tours_nearest);
    }
    for (int i = 0; i < numOfCoords; i++) {
        free(coords[i]);
        free(dMatrix[i]);
    }
    free(coords);
    free(dMatrix);

    MPI_Finalize();
    return 0;
}







int *findCheapestTour(double **dMatrix, int numOfCoords)
{
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    pathResult.totalDistance = 0.0;

    // 遍历起点
    int i = 0;
    for (int i = 0; i < numOfCoords; i++)
    {
        printf("Cheapest Point: %d\n", i);
        pathResult = cheapestInsertion(dMatrix, numOfCoords, i);
        if (pathResult.totalDistance < minDistance)
        {
            // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
            minDistance = pathResult.totalDistance;
            minTour = pathResult.tour;

            // TODO	debug
            // printf("Cheapest: ");
            // for (int i = 0; i < numOfCoords + 1; i++)
            // {
            // 	printf(" %d", minTour[i]);
            // }
            // printf("\n");
        }
    }
    // free(pathResult.tour);
    return minTour;
}

int *findFarthestTour(double **dMatrix, int numOfCoords)
{
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    pathResult.totalDistance = 0.0;

    // 遍历起点
    int i = 0;
    for (int i = 0; i < numOfCoords; i++)
    {
        printf("Farthest Point: %d\n", i);
        pathResult = farthestInsertion(dMatrix, numOfCoords, i);
        if (pathResult.totalDistance < minDistance)
        {
            // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
            minDistance = pathResult.totalDistance;
            minTour = pathResult.tour;

            // TODO	debug
            // printf("Farthest: ");
            // for (int i = 0; i < numOfCoords + 1; i++)
            // {
            // 	printf(" %d", minTour[i]);
            // }
            // printf("\n");
        }
    }
    // free(pathResult.tour);
    return minTour;
}

int *findNearestTour(double **dMatrix, int numOfCoords)
{
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    pathResult.totalDistance = 0.0;

    // 遍历起点
    int i = 0;
    for (int i = 0; i < numOfCoords; i++)
    {
        printf("Nearest Point: %d\n", i);
        pathResult = nearestAddition(dMatrix, numOfCoords, i);
        if (pathResult.totalDistance < minDistance)
        {
            // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
            minDistance = pathResult.totalDistance;
            minTour = pathResult.tour;

            // TODO	debug
            // printf("Nearest : ");
            // for (int i = 0; i < numOfCoords + 1; i++)
            // {
            // 	printf(" %d", minTour[i]);
            // }
            // printf("\n");
        }
    }

    // free(pathResult.tour);
    return minTour;
}

double **createDistanceMatrix(double **coords, int numOfCoords)
{
    int i, j;

    double **dMatrix = (double **)malloc(numOfCoords * sizeof(double));

    for (i = 0; i < numOfCoords; i++)
    {
        dMatrix[i] = (double *)malloc(numOfCoords * sizeof(double));
    }

// #pragma omp parallel for collapse(2)
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