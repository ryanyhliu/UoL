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
        int bestNextNode = -1;
        double globalMaxCost = 0;

        #pragma omp parallel 
        {
            int threadID = omp_get_thread_num();
            maxCosts[threadID] = 0;
            nextNodes[threadID] = -1;

            #pragma omp for nowait
            for(int i = 0; i < tourLength - 1; i++) {
                for(int j = 0; j < numOfCoords; j++) {
                    if(!visited[j]) {
                        double cost = dMatrix[tour[i]][j];
                        if(cost > maxCosts[threadID]) {
                            maxCosts[threadID] = cost;
                            nextNodes[threadID] = j;
                        }
                    }
                }
            }

            // 在每个线程结束后执行，无需 critical
            #pragma omp flush (globalMaxCost)
            if (maxCosts[threadID] > globalMaxCost) {
                #pragma omp critical
                {
                    if (maxCosts[threadID] > globalMaxCost) {
                        globalMaxCost = maxCosts[threadID];
                        bestNextNode = nextNodes[threadID];
                    }
                }
            }
        }

        double minCost = __DBL_MAX__;
        int insertPos = -1;

        // 不是并行的部分
        for(int k = 0; k < tourLength - 1; k++) {
            double cost = dMatrix[tour[k]][bestNextNode] + dMatrix[bestNextNode][tour[k + 1]] - dMatrix[tour[k]][tour[k + 1]];
            if(cost < minCost - tolerance) {
                minCost = cost;
                insertPos = k + 1;
            }
        }

        for(int i = numOfCoords; i > insertPos; i--) {
            tour[i] = tour[i - 1];
        }

        tour[insertPos] = bestNextNode;
        visited[bestNextNode] = true;
        totalDistance += minCost;
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
        double minCost = __DBL_MAX__;
        int bestNextNode = -1;
        int insertPos = -1;

        // OpenMP并行区域
        #pragma omp parallel for reduction(min:minCost)
        for (int j = 0; j < numOfCoords; j++) {
            if (!visited[j]) {
                for (int i = 0; i < tourLength - 1; i++) {
                    double cost = dMatrix[tour[i]][j] + dMatrix[j][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
                    #pragma omp critical
                    {
                        if (cost < minCost) {
                            minCost = cost;
                            bestNextNode = j;
                            insertPos = i + 1;
                        }
                    }
                }
            }
        }

        // 插入操作
        if (bestNextNode != -1) {
            for (int i = tourLength; i >= insertPos; i--) {
                tour[i] = tour[i - 1];
            }
            tour[insertPos] = bestNextNode;
            visited[bestNextNode] = true;
            totalDistance += minCost;
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
