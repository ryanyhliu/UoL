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
//     // Setting up variables
//     int nextNode, insertPos;

//     // Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
//     int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
//     bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

//     if (tour == NULL) {
//         printf("Memory allocation failed");
//         exit(EXIT_FAILURE);
//     }

//     // Initialising tour to empty
//     for (int i = 0; i < numOfCoords; i++) {
//         tour[i] = -1;
//     }

//     // Tour always starts and ends at 0. 0 is visited
//     tour[0] = pointOfStartEnd;
//     tour[1] = pointOfStartEnd; // Changed to place the end at the correct position
//     visited[pointOfStartEnd] = true;

//     int numVisited = 1;
//     int tourLength = 2;

// 	// TODO DEBUG check
//     // omp_set_num_threads(2);
//     // omp_set_num_threads(4);
//     // omp_set_num_threads(8);
//     // omp_set_num_threads(16);
//     int numThreads = omp_get_max_threads();
//     // printf("This program uses %d threads \n\n", numThreads);

//     double *threadMinCosts = (double *)malloc(numThreads * 8 * sizeof(double));
//     int *threadNextNode = (int *)malloc(numThreads * 8 * sizeof(int));
//     int *threadInsertPos = (int *)malloc(numThreads * 8 * sizeof(int));

//     #pragma omp parallel
//     {
//         int threadID = omp_get_thread_num();
//         while (numVisited < numOfCoords) {
//             threadMinCosts[threadID * 8] = __DBL_MAX__;
//             threadNextNode[threadID * 8] = -1;
//             threadInsertPos[threadID * 8] = -1;

//             #pragma omp for collapse(2)
//             for (int i = 0; i < tourLength - 1; i++) {
//                 for (int j = 0; j < numOfCoords; j++) {
//                     if (!visited[j]) {
//                         double cost = dMatrix[tour[i]][j] + dMatrix[tour[i + 1]][j] - dMatrix[tour[i]][tour[i + 1]];
//                         if (cost < threadMinCosts[threadID * 8]) {
//                             threadMinCosts[threadID * 8] = cost;
//                             threadNextNode[threadID * 8] = j;
//                             threadInsertPos[threadID * 8] = i + 1;
//                         }
//                     }
//                 }
//             }

//             #pragma omp single
//             {
//                 double minCost = __DBL_MAX__;
//                 for (int i = 0; i < numThreads; i++) {
//                     if (threadMinCosts[i * 8] < minCost) {
//                         minCost = threadMinCosts[i * 8];
//                         nextNode = threadNextNode[i * 8];
//                         insertPos = threadInsertPos[i * 8];
//                     }
//                 }

//                 for (int i = numOfCoords; i > insertPos; i--) {
//                     tour[i] = tour[i - 1];
//                 }

//                 tour[insertPos] = nextNode;
//                 visited[nextNode] = true;

//                 tourLength++;
//                 numVisited++;
//             }
//         }
//     }

//     // Calculate total distance
//     double totalDistance = 0.0;
//     for (int i = 0; i < numOfCoords; i++) {
//         totalDistance += dMatrix[tour[i]][tour[i + 1]];
//     }

//     // Free memory
//     free(visited);
//     free(threadMinCosts);
//     free(threadNextNode);
//     free(threadInsertPos);

//     // Prepare and return the result
//     TourResult result;
//     result.tour = tour;
//     result.totalDistance = totalDistance;
//     return result;
// }


TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
    // Setting up variables
    int nextNode = -1, insertPos = -1;

    // Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
    int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    if (tour == NULL || visited == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize tour to -1
    for (int i = 0; i <= numOfCoords; i++) {
        tour[i] = -1;
    }

    // Start and end at pointOfStartEnd
    tour[0] = pointOfStartEnd;
    tour[1] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;

    int numVisited = 1;
    int tourLength = 2;

    // Get the number of threads
    int numThreads = omp_get_max_threads();

    // Allocate memory for thread-local data
    double *threadMinCosts = (double *)malloc(numThreads * sizeof(double));
    int *threadNextNodes = (int *)malloc(numThreads * sizeof(int));
    int *threadInsertPos = (int *)malloc(numThreads * sizeof(int));

    while (numVisited < numOfCoords) {
        // Initialize thread-local min costs
        #pragma omp parallel for
        for (int i = 0; i < numThreads; i++) {
            threadMinCosts[i] = __DBL_MAX__;
            threadNextNodes[i] = -1;
            threadInsertPos[i] = -1;
        }

        // Find the best insertion in parallel
        #pragma omp parallel
        {
            int threadID = omp_get_thread_num();
            double minCost = __DBL_MAX__;
            int bestNode = -1, bestPos = -1;

            #pragma omp for nowait
            for (int i = 0; i < tourLength - 1; i++) {
                for (int j = 0; j < numOfCoords; j++) {
                    if (!visited[j]) {
                        double cost = dMatrix[tour[i]][j] + dMatrix[tour[i + 1]][j] - dMatrix[tour[i]][tour[i + 1]];
                        if (cost < minCost) {
                            minCost = cost;
                            bestNode = j;
                            bestPos = i + 1;
                        }
                    }
                }
            }

            threadMinCosts[threadID] = minCost;
            threadNextNodes[threadID] = bestNode;
            threadInsertPos[threadID] = bestPos;
        }

        // Find global minimum cost outside of parallel region
        double globalMinCost = __DBL_MAX__;
        for (int i = 0; i < numThreads; i++) {
            if (threadMinCosts[i] < globalMinCost) {
                globalMinCost = threadMinCosts[i];
                nextNode = threadNextNodes[i];
                insertPos = threadInsertPos[i];
            }
        }

        // Insert the best node into the tour
        if (nextNode != -1) {
            for (int i = tourLength; i > insertPos; i--) {
                tour[i] = tour[i - 1];
            }
            tour[insertPos] = nextNode;
            visited[nextNode] = true;

            tourLength++;
            numVisited++;
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
    free(threadNextNodes);
    free(threadInsertPos);

    // Prepare and return the result
    TourResult result;
    result.tour = tour;
    result.totalDistance = totalDistance;
    return result;
}