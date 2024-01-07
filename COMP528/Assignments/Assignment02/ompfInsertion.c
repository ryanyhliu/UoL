#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include<omp.h>


// typedef struct {
//     int *tour;
//     double totalDistance;
// } TourResult;


double getDistance_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
    double tolerance = 1e-9;


    int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
    double totalDistance = 0.0;

    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    for(int i = 0; i < numOfCoords; i++) {
        tour[i] = -1;
    }

    tour[0] = pointOfStartEnd;
    tour[1] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;

    int numVisited = 1;
    int tourLength = 2;

    int numThreads = omp_get_max_threads();
    double *maxCosts = (double *)malloc(numThreads * sizeof(double));
    int *nextNodes = (int *)malloc(numThreads * sizeof(int));

    while(numVisited < numOfCoords) {
        double globalMinCost = __DBL_MAX__;
        int globalBestNextNode = -1;
        int globalInsertPos = -1;

        #pragma omp parallel 
        {
            double localMinCost = __DBL_MAX__;
            int localBestNextNode = -1;
            int localInsertPos = -1;

            #pragma omp for nowait
            for (int j = 0; j < numOfCoords; j++) {
                if (!visited[j]) {
                    for (int i = 0; i < tourLength - 1; i++) {
                        double cost = dMatrix[tour[i]][j] + dMatrix[j][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
                        if (cost < localMinCost) {
                            localMinCost = cost;
                            localBestNextNode = j;
                            localInsertPos = i + 1;
                        }
                    }
                }
            }

            #pragma omp critical
            {
                if (localMinCost < globalMinCost) {
                    globalMinCost = localMinCost;
                    globalBestNextNode = localBestNextNode;
                    globalInsertPos = localInsertPos;
                }
            }
        }

        // 插入操作
        if (globalBestNextNode != -1) {
            for (int i = tourLength; i >= globalInsertPos; i--) {
                tour[i] = tour[i - 1];
            }
            tour[globalInsertPos] = globalBestNextNode;
            visited[globalBestNextNode] = true;
            totalDistance += globalMinCost;
            tourLength++;
            numVisited++;
        }
    }

    free(maxCosts);
    free(nextNodes);
    free(visited);

    // TourResult result;
    // result.tour = tour;
    // result.totalDistance = totalDistance;
    return totalDistance;
}



int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
    double tolerance = 1e-9;


    int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    for(int i = 0; i < numOfCoords; i++) {
        tour[i] = -1;
    }

    tour[0] = pointOfStartEnd;
    tour[1] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;

    int numVisited = 1;
    int tourLength = 2;
    double totalDistance = 0.0;

    int numThreads = omp_get_max_threads();
    double *maxCosts = (double *)malloc(numThreads * sizeof(double));
    int *nextNodes = (int *)malloc(numThreads * sizeof(int));

    while(numVisited < numOfCoords) {
        double globalMinCost = __DBL_MAX__;
        int globalBestNextNode = -1;
        int globalInsertPos = -1;

        #pragma omp parallel 
        {
            double localMinCost = __DBL_MAX__;
            int localBestNextNode = -1;
            int localInsertPos = -1;

            #pragma omp for nowait
            for (int j = 0; j < numOfCoords; j++) {
                if (!visited[j]) {
                    for (int i = 0; i < tourLength - 1; i++) {
                        double cost = dMatrix[tour[i]][j] + dMatrix[j][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
                        if (cost < localMinCost) {
                            localMinCost = cost;
                            localBestNextNode = j;
                            localInsertPos = i + 1;
                        }
                    }
                }
            }

            #pragma omp critical
            {
                if (localMinCost < globalMinCost) {
                    globalMinCost = localMinCost;
                    globalBestNextNode = localBestNextNode;
                    globalInsertPos = localInsertPos;
                }
            }
        }

        // 插入操作
        if (globalBestNextNode != -1) {
            for (int i = tourLength; i >= globalInsertPos; i--) {
                tour[i] = tour[i - 1];
            }
            tour[globalInsertPos] = globalBestNextNode;
            visited[globalBestNextNode] = true;
            totalDistance += globalMinCost;
            tourLength++;
            numVisited++;
        }
    }


    free(maxCosts);
    free(nextNodes);
    free(visited);

    // TourResult result;
    // result.tour = tour;
    // result.totalDistance = totalDistance;
    return tour;
}
