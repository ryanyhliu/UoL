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
    
    // Initialize tour to -1 indicating unvisited
    for (int i = 0; i <= numOfCoords; ++i) {
        tour[i] = -1;
    }

    // Start tour at the first node
    int startNode = 0; // Assuming the first node as the start node
    tour[0] = startNode;
    tour[numOfCoords] = startNode; // End at the start node to make a loop
    visited[startNode] = true;

    int numVisited = 1;
    int tourLength = 1; // Current length of the tour

    while (numVisited < numOfCoords) {
        double maxDistance = -1.0;
        int farthestNode = -1;

        // Find the farthest unvisited node
        #pragma omp parallel for
        for (int i = 0; i < numOfCoords; ++i) {
            if (!visited[i]) {
                double distance = 0.0;
                for (int j = 0; j < tourLength; ++j) {
                    distance += dMatrix[tour[j]][i];
                }

                #pragma omp critical
                {
                    if (distance > maxDistance) {
                        maxDistance = distance;
                        farthestNode = i;
                    }
                }
            }
        }

        // Find the position where the farthest node will be inserted
        double minInsertionCost = DBL_MAX;
        int insertPosition = -1;
        for (int i = 0; i < tourLength; ++i) {
            int nextIndex = (i + 1) % tourLength;
            double cost = dMatrix[tour[i]][farthestNode] + dMatrix[farthestNode][tour[nextIndex]] - dMatrix[tour[i]][tour[nextIndex]];

            if (cost < minInsertionCost) {
                minInsertionCost = cost;
                insertPosition = i + 1;
            }
        }

        // Insert the farthest node at the found position
        for (int i = tourLength; i >= insertPosition; --i) {
            tour[i + 1] = tour[i];
        }
        tour[insertPosition] = farthestNode;
        visited[farthestNode] = true;
        totalDistance += minInsertionCost;
        numVisited++;
        tourLength++;
    }

    // Connect the last node to the start node
    totalDistance += dMatrix[tour[tourLength - 1]][tour[0]];

    // Clean up
    free(tour);
    free(visited);

    // Return the total distance of the tour
    return totalDistance;
}




int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords) {
    int *tour = malloc((1 + numOfCoords) * sizeof(int));
    bool *visited = calloc(numOfCoords, sizeof(bool));
    int tourLength = 1;  // Starting with the first point
    
    // Initialize tour with -1 indicating unvisited
    for (int i = 0; i <= numOfCoords; ++i) {
        tour[i] = -1;
    }

    // Start the tour at the first point (assuming 0 as the start)
    tour[0] = 0;
    visited[0] = true;
    
    // Continue until all nodes are visited
    while (tourLength < numOfCoords) {
        double maxDistance = -1.0;
        int farthestNode = -1;

        // Parallel section to find the farthest unvisited node
        #pragma omp parallel
        {
            int threadID = omp_get_thread_num();
            double threadMaxDistance = -1.0;
            int threadFarthestNode = -1;

            #pragma omp for nowait
            for (int i = 0; i < numOfCoords; ++i) {
                if (!visited[i]) {
                    double nodeDistance = 0.0;
                    for (int j = 0; j < tourLength; ++j) {
                        nodeDistance += dMatrix[tour[j]][i];
                    }

                    #pragma omp critical
                    {
                        if (nodeDistance > threadMaxDistance) {
                            threadMaxDistance = nodeDistance;
                            threadFarthestNode = i;
                        }
                    }
                }
            }

            // Critical section to update the global farthest node
            #pragma omp critical
            {
                if (threadMaxDistance > maxDistance) {
                    maxDistance = threadMaxDistance;
                    farthestNode = threadFarthestNode;
                }
            }
        }

        // Find the best insertion position for the farthest node
        double minInsertionCost = DBL_MAX;
        int bestInsertPos = -1;
        for (int i = 0; i < tourLength; ++i) {
            int nextIndex = (i + 1) % tourLength;
            double insertionCost = dMatrix[tour[i]][farthestNode] + dMatrix[farthestNode][tour[nextIndex]] - dMatrix[tour[i]][tour[nextIndex]];
            if (insertionCost < minInsertionCost) {
                minInsertionCost = insertionCost;
                bestInsertPos = i + 1;
            }
        }

        // Insert the farthest node at the best position
        for (int i = tourLength; i >= bestInsertPos; --i) {
            tour[i] = tour[i - 1];
        }
        tour[bestInsertPos] = farthestNode;
        visited[farthestNode] = true;
        tourLength++;
    }

    // Add the start point at the end to complete the tour
    tour[numOfCoords] = tour[0];

    // Clean up
    free(visited);

    // Return the tour
    return tour;
}