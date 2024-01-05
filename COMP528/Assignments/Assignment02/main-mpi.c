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

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char filename[500];
    char outFileName_Cheapest[500];
    char outFileName_Farthest[500];
    char outFileName_Nearest[500];

    strcpy(filename, argv[1]);
    strcpy(outFileName_Cheapest, argv[2]);
    strcpy(outFileName_Farthest, argv[3]);
    strcpy(outFileName_Nearest, argv[4]);

    if (filename == NULL || outFileName_Cheapest == NULL || outFileName_Farthest == NULL || outFileName_Nearest == NULL)
    {
        printf("Error reading files");
        return 1;
    }

    int numOfCoords = readNumOfCoords(filename);

    double **coords = readCoords(filename, numOfCoords);

    double tStart = omp_get_wtime();

    double **dMatrix = createDistanceMatrix(coords, numOfCoords);

    int *tourCheapest = findCheapestTour(dMatrix, numOfCoords);
    int *tourFarthest = findFarthestTour(dMatrix, numOfCoords);
    int *tourNearest = findNearestTour(dMatrix, numOfCoords);

    double tEnd = omp_get_wtime();

    printf("Took %f milliseconds\n", (tEnd - tStart) * 1000);

    if (writeTourToFile(tourCheapest, numOfCoords + 1, outFileName_Cheapest) == NULL)
    {
        printf("Error");
    }
    if (writeTourToFile(tourFarthest, numOfCoords + 1, outFileName_Farthest) == NULL)
    {
        printf("Error");
    }
    if (writeTourToFile(tourNearest, numOfCoords + 1, outFileName_Nearest) == NULL)
    {
        printf("Error");
    }

    for (int i = 0; i < numOfCoords; i++)
    {
        free(dMatrix[i]);
    }

    free(dMatrix);
    free(tourCheapest);
    free(tourFarthest);
    free(tourNearest);
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
