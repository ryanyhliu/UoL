#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <omp.h>

typedef struct {
    int *tour;
    double totalDistance;
} TourResult;





// TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
//     TourResult result;
//     result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
//     result.totalDistance = 0.0;

//     int *tour = result.tour;
//     bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

//     tour[0] = pointOfStartEnd;
//     tour[1] = pointOfStartEnd;
//     visited[pointOfStartEnd] = true;

//     int numVisited = 1;
//     int tourLength = 2;

//     while (numVisited < numOfCoords) {
//         double minCost = __DBL_MAX__;
//         int bestNextNode = -1;
//         int bestInsertPos = -1;

//         #pragma omp parallel
//         {
//             double localMinCost = __DBL_MAX__;
//             int localNextNode = -1;
//             int localInsertPos = -1;

//             #pragma omp for collapse(2) nowait
//             for (int i = 0; i < tourLength - 1; i++) {
//                 for (int j = 0; j < numOfCoords; j++) {
//                     if (!visited[j]) {
//                         double cost = dMatrix[tour[i]][j] + dMatrix[j][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
//                         if (cost < localMinCost) {
//                             localMinCost = cost;
//                             localNextNode = j;
//                             localInsertPos = i + 1;
//                         }
//                     }
//                 }
//             }

//             #pragma omp critical
//             {
//                 if (localMinCost < minCost) {
//                     minCost = localMinCost;
//                     bestNextNode = localNextNode;
//                     bestInsertPos = localInsertPos;
//                 }
//             }
//         }

//         if (bestNextNode != -1) {
//             for (int i = numOfCoords; i > bestInsertPos; i--) {
//                 tour[i] = tour[i - 1];
//             }

//             tour[bestInsertPos] = bestNextNode;
//             visited[bestNextNode] = true;
//             result.totalDistance += minCost;

//             tourLength++;
//             numVisited++;
//         }
//     }

//     free(visited);

//     return result;
// }






// Implementation of the cheapestInsertion function
TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
    // Setting up variables
    int nextNode, insertPos;

    // Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
    int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    if (tour == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Initialising tour to empty
    for (int i = 0; i < numOfCoords; i++) {
        tour[i] = -1;
    }

    // Tour always starts and ends at 0. 0 is visited
    tour[0] = pointOfStartEnd;
    tour[1] = pointOfStartEnd; // Changed to place the end at the correct position
    visited[pointOfStartEnd] = true;

    int numVisited = 1;
    int tourLength = 2;

    int numThreads = omp_get_max_threads();
    // printf("This program uses %d threads \n\n", numThreads);

    double *threadMinCosts = (double *)malloc(numThreads * 8 * sizeof(double));
    int *threadNextNode = (int *)malloc(numThreads * 8 * sizeof(int));
    int *threadInsertPos = (int *)malloc(numThreads * 8 * sizeof(int));

    #pragma omp parallel
    {
        int threadID = omp_get_thread_num();
        while (numVisited < numOfCoords) {
            threadMinCosts[threadID * 8] = __DBL_MAX__;
            threadNextNode[threadID * 8] = -1;
            threadInsertPos[threadID * 8] = -1;

            #pragma omp for collapse(2)
            for (int i = 0; i < tourLength - 1; i++) {
                for (int j = 0; j < numOfCoords; j++) {
                    if (!visited[j]) {
                        double cost = dMatrix[tour[i]][j] + dMatrix[tour[i + 1]][j] - dMatrix[tour[i]][tour[i + 1]];
                        if (cost < threadMinCosts[threadID * 8]) {
                            threadMinCosts[threadID * 8] = cost;
                            threadNextNode[threadID * 8] = j;
                            threadInsertPos[threadID * 8] = i + 1;
                        }
                    }
                }
            }

            #pragma omp single
            {
                double minCost = __DBL_MAX__;
                for (int i = 0; i < numThreads; i++) {
                    if (threadMinCosts[i * 8] < minCost) {
                        minCost = threadMinCosts[i * 8];
                        nextNode = threadNextNode[i * 8];
                        insertPos = threadInsertPos[i * 8];
                    }
                }

                for (int i = numOfCoords; i > insertPos; i--) {
                    tour[i] = tour[i - 1];
                }

                tour[insertPos] = nextNode;
                visited[nextNode] = true;

                tourLength++;
                numVisited++;
            }
        }
    }

    // Calculate total distance
    double totalDistance = 0.0;
    for (int i = 0; i < numOfCoords; i++) {
        totalDistance += dMatrix[tour[i]][tour[i + 1]];
    }

    // Free memory
    free(visited);
    free(threadMinCosts);
    free(threadNextNode);
    free(threadInsertPos);

    // Prepare and return the result
    TourResult result;
    result.tour = tour;
    result.totalDistance = totalDistance;
    return result;
}

