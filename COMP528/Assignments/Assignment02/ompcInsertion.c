#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <omp.h>




// TOMB FOR MY ORIGINAL IDEA, I THINK THIS CODE IS MORE REASONABLE THAN FINAL VERSION


// typedef struct {
//     int *tour;
//     double totalDistance;
// } TourResult;



double getDistance_CheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
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

    // TourResult result;
    // result.tour = tour;
    // result.totalDistance = totalDistance;
    return totalDistance;
}

int *getTour_CheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
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

    // TourResult result;
    // result.tour = tour;
    // result.totalDistance = totalDistance;
    return tour;
}



