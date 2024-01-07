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
    if (numOfCoords <= 0) {
        return 0.0;  // No distance to calculate for empty or invalid input.
    }

    int *tour = malloc((numOfCoords + 1) * sizeof(int));  // +1 for returning to the start
    if (!tour) {
        perror("Failed to allocate memory for tour");
        exit(EXIT_FAILURE);
    }

    bool *visited = calloc(numOfCoords, sizeof(bool));
    if (!visited) {
        perror("Failed to allocate memory for visited");
        free(tour);
        exit(EXIT_FAILURE);
    }

    double totalDistance = 0.0;
    int tourLength = 1;  // Starting with the first point

    // Initialize tour with -1 indicating unvisited
    for (int i = 0; i < numOfCoords; ++i) {
        tour[i] = -1;
    }
    tour[0] = 0;  // Assuming 0 as the start node
    visited[0] = true;

    int numVisited = 1;
    while (numVisited < numOfCoords) {
        double globalMaxDistance = -__DBL_MAX__;
        int globalFarthestNode = -1;

        // Parallel section to find the farthest unvisited node
        #pragma omp parallel
        {
            double localMaxDistance = -__DBL_MAX__;
            int localFarthestNode = -1;

            #pragma omp for nowait
            for (int i = 0; i < numOfCoords; ++i) {
                if (!visited[i]) {
                    double nodeDistance = 0.0;
                    for (int j = 0; j < tourLength; ++j) {
                        nodeDistance += dMatrix[tour[j]][i];
                    }

                    #pragma omp critical
                    {
                        if (nodeDistance > localMaxDistance) {
                            localMaxDistance = nodeDistance;
                            localFarthestNode = i;
                        }
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

        // Find the best position to insert the farthest node
        double minInsertionCost = __DBL_MAX__;
        int bestInsertPos = -1;
        for (int i = 0; i < tourLength - 1; ++i) {
            double cost = dMatrix[tour[i]][globalFarthestNode] + dMatrix[globalFarthestNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (cost < minInsertionCost) {
                minInsertionCost = cost;
                bestInsertPos = i + 1;
            }
        }

        // Insert the farthest node at the best position
        for (int i = tourLength; i >= bestInsertPos; --i) {
            tour[i] = tour[i - 1];
        }
        tour[bestInsertPos] = globalFarthestNode;
        visited[globalFarthestNode] = true;
        totalDistance += minInsertionCost;
        numVisited++;
        tourLength++;
    }

    // Add distance back to the start node, if applicable
    if (tourLength > 1 && tour[tourLength - 1] != -1) {
        totalDistance += dMatrix[tour[tourLength - 1]][tour[0]];
    }

    free(tour);
    free(visited);

    return totalDistance;
}





int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords) {
    if (numOfCoords <= 0) {
        return 0.0;  // No distance to calculate for empty or invalid input.
    }

    int *tour = malloc((numOfCoords + 1) * sizeof(int));  // +1 for returning to the start
    if (!tour) {
        perror("Failed to allocate memory for tour");
        exit(EXIT_FAILURE);
    }

    bool *visited = calloc(numOfCoords, sizeof(bool));
    if (!visited) {
        perror("Failed to allocate memory for visited");
        free(tour);
        exit(EXIT_FAILURE);
    }

    double totalDistance = 0.0;
    int tourLength = 1;  // Starting with the first point

    // Initialize tour with -1 indicating unvisited
    for (int i = 0; i < numOfCoords; ++i) {
        tour[i] = -1;
    }
    tour[0] = 0;  // Assuming 0 as the start node
    visited[0] = true;

    int numVisited = 1;
    while (numVisited < numOfCoords) {
        double globalMaxDistance = -__DBL_MAX__;
        int globalFarthestNode = -1;

        // Parallel section to find the farthest unvisited node
        #pragma omp parallel
        {
            double localMaxDistance = -__DBL_MAX__;
            int localFarthestNode = -1;

            #pragma omp for nowait
            for (int i = 0; i < numOfCoords; ++i) {
                if (!visited[i]) {
                    double nodeDistance = 0.0;
                    for (int j = 0; j < tourLength; ++j) {
                        nodeDistance += dMatrix[tour[j]][i];
                    }

                    #pragma omp critical
                    {
                        if (nodeDistance > localMaxDistance) {
                            localMaxDistance = nodeDistance;
                            localFarthestNode = i;
                        }
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

        // Find the best position to insert the farthest node
        double minInsertionCost = __DBL_MAX__;
        int bestInsertPos = -1;
        for (int i = 0; i < tourLength - 1; ++i) {
            double cost = dMatrix[tour[i]][globalFarthestNode] + dMatrix[globalFarthestNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (cost < minInsertionCost) {
                minInsertionCost = cost;
                bestInsertPos = i + 1;
            }
        }

        // Insert the farthest node at the best position
        for (int i = tourLength; i >= bestInsertPos; --i) {
            tour[i] = tour[i - 1];
        }
        tour[bestInsertPos] = globalFarthestNode;
        visited[globalFarthestNode] = true;
        totalDistance += minInsertionCost;
        numVisited++;
        tourLength++;
    }

    // Add distance back to the start node, if applicable
    if (tourLength > 1 && tour[tourLength - 1] != -1) {
        totalDistance += dMatrix[tour[tourLength - 1]][tour[0]];
    }

    // free(tour);
    free(visited);

    return tour;
}