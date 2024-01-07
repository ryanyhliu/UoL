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


double getDistance_FarthestInsertion(double **dMatrix, int numOfCoords) {
    int *tour = malloc((numOfCoords + 1) * sizeof(int));
    bool *visited = calloc(numOfCoords, sizeof(bool));
    double totalDistance = 0.0;
    int numVisited = 1;
    int tourLength = 2;  // Starting with the first point
    
    for (int i = 0; i <= numOfCoords; ++i) {
        tour[i] = -1;
    }

    tour[0] = 0;  // Assuming 0 as the start node
    tour[1] = 0;  // Loop back to the start
    visited[0] = true;

    int numThreads = omp_get_max_threads();
    double *threadMaxCosts = malloc(numThreads * sizeof(double));
    int *threadFarthestNodes = malloc(numThreads * sizeof(int));

    while (numVisited < numOfCoords) {
        double globalMaxDistance = -1.0;
        int globalFarthestNode = -1;

        #pragma omp parallel
        {
            int threadID = omp_get_thread_num();
            double localMaxDistance = -1.0;
            int localFarthestNode = -1;

            #pragma omp for nowait
            for (int i = 0; i < numOfCoords; ++i) {
                if (!visited[i]) {
                    double nodeDistance = 0.0;
                    for (int j = 0; j < tourLength; ++j) {
                        nodeDistance += dMatrix[tour[j]][i];
                    }
                    if (nodeDistance > localMaxDistance) {
                        localMaxDistance = nodeDistance;
                        localFarthestNode = i;
                    }
                }
            }

            #pragma omp critical
            {
                if (localMaxDistance > globalMaxDistance) {
                    globalMaxDistance = localMaxDistance;
                    globalFarthestNode = localFarthestNode;
                }
            }
        }

        double minInsertionCost = __DBL_MAX__;
        int bestInsertPos = -1;
        for (int i = 0; i < tourLength - 1; ++i) {
            double cost = dMatrix[tour[i]][globalFarthestNode] + dMatrix[globalFarthestNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (cost < minInsertionCost) {
                minInsertionCost = cost;
                bestInsertPos = i + 1;
            }
        }

        for (int i = tourLength; i >= bestInsertPos; --i) {
            tour[i] = tour[i - 1];
        }
        tour[bestInsertPos] = globalFarthestNode;
        visited[globalFarthestNode] = true;
        totalDistance += minInsertionCost;
        numVisited++;
        tourLength++;
    }

    // Add distance back to the start node
    totalDistance += dMatrix[tour[tourLength - 1]][tour[0]];

    free(tour);
    free(visited);
    free(threadMaxCosts);
    free(threadFarthestNodes);

    return totalDistance;
}





int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords) {
    int *tour = malloc((numOfCoords + 1) * sizeof(int));
    bool *visited = calloc(numOfCoords, sizeof(bool));
    int numVisited = 1;
    int tourLength = 2;  // Starting with the first point
    
    // Initialize tour with -1 indicating unvisited
    for (int i = 0; i < numOfCoords; ++i) {
        tour[i] = -1;
    }

    // Start the tour at the first point (assuming 0 as the start)
    tour[0] = 0;
    tour[numOfCoords] = 0; // End at the start to make a loop
    visited[0] = true;
    
    // Get the number of threads
    int numThreads = omp_get_max_threads();
    
    // Allocate memory for local data per thread to prevent false sharing
    double *threadMaxCosts = malloc(numThreads * sizeof(double));
    int *threadFarthestNodes = malloc(numThreads * sizeof(int));
    
    while (numVisited < numOfCoords) {
        // Find the farthest unvisited node
        #pragma omp parallel
        {
            int threadID = omp_get_thread_num();
            double maxDistance = -__DBL_MAX__;
            int farthestNode = -1;
            #pragma omp for nowait
            for (int i = 0; i < numOfCoords; ++i) {
                if (!visited[i]) {
                    double nodeDistance = 0.0;
                    for (int j = 0; j < tourLength; ++j) {
                        nodeDistance += dMatrix[tour[j]][i];
                    }
                    if (nodeDistance > maxDistance) {
                        maxDistance = nodeDistance;
                        farthestNode = i;
                    }
                }
            }
            threadMaxCosts[threadID] = maxDistance;
            threadFarthestNodes[threadID] = farthestNode;
        }

        // Reduce the results to find the overall farthest node
        double globalMaxDistance = -__DBL_MAX__;
        int globalFarthestNode = -1;
        for (int i = 0; i < numThreads; ++i) {
            if (threadMaxCosts[i] > globalMaxDistance) {
                globalMaxDistance = threadMaxCosts[i];
                globalFarthestNode = threadFarthestNodes[i];
            }
        }
        
        // Find the best position to insert the farthest node
        double minInsertionCost = __DBL_MAX__;
        int bestInsertPos = -1;
        for (int i = 0; i < tourLength - 1; ++i) {
            double insertionCost = dMatrix[tour[i]][globalFarthestNode] + dMatrix[globalFarthestNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (insertionCost < minInsertionCost) {
                minInsertionCost = insertionCost;
                bestInsertPos = i + 1;
            }
        }

        // Insert the farthest node at the best position
        for (int i = tourLength; i >= bestInsertPos; --i) {
            tour[i] = tour[i - 1];
        }
        tour[bestInsertPos] = globalFarthestNode;
        visited[globalFarthestNode] = true;
        numVisited++;
        tourLength++;
    }

    // Clean up
    free(visited);
    free(threadMaxCosts);
    free(threadFarthestNodes);

    // Return the tour
    return tour;
}