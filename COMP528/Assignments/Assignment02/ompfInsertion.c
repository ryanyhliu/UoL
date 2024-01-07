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
        int globalFarthestNextNode = -1;
        double globalMaxDistance = -1;

        // 找到最远的未访问节点
        #pragma omp parallel for
        for (int i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                double localMaxDistance = -1;
                for (int j = 0; j < tourLength; j++) {
                    double distance = dMatrix[tour[j]][i];
                    if (distance > localMaxDistance) {
                        localMaxDistance = distance;
                    }
                }

                #pragma omp critical
                {
                    if (localMaxDistance > globalMaxDistance) {
                        globalMaxDistance = localMaxDistance;
                        globalFarthestNextNode = i;
                    }
                }
            }
        }

        // 寻找插入最远节点的最佳位置
        double minInsertionCost = __DBL_MAX__;
        int insertPos = -1;
        for (int i = 0; i < tourLength - 1; i++) {
            double cost = dMatrix[tour[i]][globalFarthestNextNode] + dMatrix[globalFarthestNextNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (cost < minInsertionCost) {
                minInsertionCost = cost;
                insertPos = i + 1;
            }
        }

        // 插入最远节点
        for (int i = tourLength; i > insertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[insertPos] = globalFarthestNextNode;
        visited[globalFarthestNextNode] = true;
        totalDistance += minInsertionCost; // 这里应该是minInsertionCost，因为它代表了插入成本
        tourLength++;
        numVisited++;
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
        int globalFarthestNextNode = -1;
        double globalMaxDistance = -1;

        // 找到最远的未访问节点
        #pragma omp parallel for
        for (int i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                double localMaxDistance = -1;
                for (int j = 0; j < tourLength; j++) {
                    double distance = dMatrix[tour[j]][i];
                    if (distance > localMaxDistance) {
                        localMaxDistance = distance;
                    }
                }

                #pragma omp critical
                {
                    if (localMaxDistance > globalMaxDistance) {
                        globalMaxDistance = localMaxDistance;
                        globalFarthestNextNode = i;
                    }
                }
            }
        }

        // 寻找插入最远节点的最佳位置
        double minInsertionCost = __DBL_MAX__;
        int insertPos = -1;
        for (int i = 0; i < tourLength - 1; i++) {
            double cost = dMatrix[tour[i]][globalFarthestNextNode] + dMatrix[globalFarthestNextNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (cost < minInsertionCost) {
                minInsertionCost = cost;
                insertPos = i + 1;
            }
        }

        // 插入最远节点
        for (int i = tourLength; i > insertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[insertPos] = globalFarthestNextNode;
        visited[globalFarthestNextNode] = true;
        totalDistance += minInsertionCost; // 这里应该是minInsertionCost，因为它代表了插入成本
        tourLength++;
        numVisited++;
    }


    free(maxCosts);
    free(nextNodes);
    free(visited);

    // TourResult result;
    // result.tour = tour;
    // result.totalDistance = totalDistance;
    return tour;
}
