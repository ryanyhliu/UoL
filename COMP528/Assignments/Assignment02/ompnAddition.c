#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<omp.h>
#include<string.h>

// typedef struct {
//     int *tour;
//     double totalDistance;
// } TourResult;


double getDistance_NearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
    // TourResult result;
    // result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    // result.totalDistance = 0.0;

    int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
    double totalDistance = 0.0;

    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    tour[0] = pointOfStartEnd;
    tour[1] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;

    int numVisited = 1;
    int tourLength = 2;

    while(numVisited < numOfCoords) {
        double minDistance = __DBL_MAX__;
        int nearestNode = -1;
        int insertPos = -1;
        double thisDistance = 0.0;

        // 声明 j 在这里，以便在并行区块中使用
        int j;

        #pragma omp parallel private(j)
        {
            double localMinDistance = __DBL_MAX__;
            int localNearestNode = -1;
            int localInsertPos = -1;
            double localThisDistance = 0.0;

            #pragma omp for nowait
            for (int i = 0; i < numOfCoords; i++) {
                if (!visited[i]) {
                    for (j = 1; j < tourLength; j++) {
                        double distance = dMatrix[tour[j]][i];
                        if (distance < localMinDistance) {
                            localMinDistance = distance;
                            localNearestNode = i;

                            // 决定插入位置的逻辑
                            if (j == tourLength - 1) {
                                double distanceToEnd = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                                double distanceToStart = dMatrix[tour[0]][i] + dMatrix[i][tour[1]] - dMatrix[tour[0]][tour[1]];
                                if (distanceToStart <= distanceToEnd) {
                                    localInsertPos = 1;
                                    localThisDistance = distanceToStart;
                                } else {
                                    localInsertPos = j;
                                    localThisDistance = distanceToEnd;
                                }
                            } else {
                                double distanceBefore = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                                double distanceAfter = dMatrix[i][tour[j]] + dMatrix[i][tour[j + 1]] - dMatrix[tour[j]][tour[j + 1]];
                                if (distanceBefore <= distanceAfter) {
                                    localInsertPos = j;
                                    localThisDistance = distanceBefore;
                                } else {
                                    localInsertPos = j + 1;
                                    localThisDistance = distanceAfter;
                                }
                            }
                        }
                    }
                }
            }

            #pragma omp critical
            {
                if (localMinDistance < minDistance) {
                    minDistance = localMinDistance;
                    nearestNode = localNearestNode;
                    insertPos = localInsertPos;
                    thisDistance = localThisDistance;
                }
            }
        }

        // 更新路径和访问状态
        for (int i = tourLength; i > insertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[insertPos] = nearestNode;
        visited[nearestNode] = true;
        numVisited++;
        tourLength++;
        totalDistance += thisDistance;
    }

    free(visited);

    return totalDistance;
}


int *getTour_NearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
    // TourResult result;
    // result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    // result.totalDistance = 0.0;

    int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
    double totalDistance = 0.0;

    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    tour[0] = pointOfStartEnd;
    tour[1] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;

    int numVisited = 1;
    int tourLength = 2;

    while(numVisited < numOfCoords) {
        double minDistance = __DBL_MAX__;
        int nearestNode = -1;
        int insertPos = -1;
        double thisDistance = 0.0;

        // 声明 j 在这里，以便在并行区块中使用
        int j;

        #pragma omp parallel private(j)
        {
            double localMinDistance = __DBL_MAX__;
            int localNearestNode = -1;
            int localInsertPos = -1;
            double localThisDistance = 0.0;

            #pragma omp for nowait
            for (int i = 0; i < numOfCoords; i++) {
                if (!visited[i]) {
                    for (j = 1; j < tourLength; j++) {
                        double distance = dMatrix[tour[j]][i];
                        if (distance < localMinDistance) {
                            localMinDistance = distance;
                            localNearestNode = i;

                            // 决定插入位置的逻辑
                            if (j == tourLength - 1) {
                                double distanceToEnd = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                                double distanceToStart = dMatrix[tour[0]][i] + dMatrix[i][tour[1]] - dMatrix[tour[0]][tour[1]];
                                if (distanceToStart <= distanceToEnd) {
                                    localInsertPos = 1;
                                    localThisDistance = distanceToStart;
                                } else {
                                    localInsertPos = j;
                                    localThisDistance = distanceToEnd;
                                }
                            } else {
                                double distanceBefore = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                                double distanceAfter = dMatrix[i][tour[j]] + dMatrix[i][tour[j + 1]] - dMatrix[tour[j]][tour[j + 1]];
                                if (distanceBefore <= distanceAfter) {
                                    localInsertPos = j;
                                    localThisDistance = distanceBefore;
                                } else {
                                    localInsertPos = j + 1;
                                    localThisDistance = distanceAfter;
                                }
                            }
                        }
                    }
                }
            }

            #pragma omp critical
            {
                if (localMinDistance < minDistance) {
                    minDistance = localMinDistance;
                    nearestNode = localNearestNode;
                    insertPos = localInsertPos;
                    thisDistance = localThisDistance;
                }
            }
        }

        // 更新路径和访问状态
        for (int i = tourLength; i > insertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[insertPos] = nearestNode;
        visited[nearestNode] = true;
        numVisited++;
        tourLength++;
        totalDistance += thisDistance;
    }

    free(visited);

    return tour;
}
