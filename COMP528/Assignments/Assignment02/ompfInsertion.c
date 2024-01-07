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

    while (numVisited < numOfCoords) {
        int farthestNode = -1;
        double farthestNodeDistance = -1;

        // Step 1: Find the farthest unvisited node
        #pragma omp parallel for
        for (int i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                double nodeDistance = 0.0;
                for (int j = 0; j < tourLength; j++) {
                    nodeDistance += dMatrix[tour[j]][i];
                }

                #pragma omp critical
                {
                    if (nodeDistance > farthestNodeDistance) {
                        farthestNodeDistance = nodeDistance;
                        farthestNode = i;
                    }
                }
            }
        }

        // Step 2: Find the best insertion position for the farthest node
        double bestInsertionCost = __DBL_MAX__;
        int bestInsertPos = -1;
        for (int i = 0; i < tourLength - 1; i++) {
            double insertionCost = dMatrix[tour[i]][farthestNode] + dMatrix[farthestNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (insertionCost < bestInsertionCost) {
                bestInsertionCost = insertionCost;
                bestInsertPos = i + 1;
            }
        }

        // Step 3: Insert the farthest node at the best position
        for (int i = tourLength; i > bestInsertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[bestInsertPos] = farthestNode;
        visited[farthestNode] = true;
        totalDistance += bestInsertionCost; // Add the cost of inserting the farthest node
        tourLength++;
        numVisited++;
    }

    free(visited);
    free(tour);

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

    while (numVisited < numOfCoords) {
        int farthestNode = -1;
        double farthestNodeDistance = -1;

        // Step 1: Find the farthest unvisited node
        #pragma omp parallel for
        for (int i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                double nodeDistance = 0.0;
                for (int j = 0; j < tourLength; j++) {
                    nodeDistance += dMatrix[tour[j]][i];
                }

                #pragma omp critical
                {
                    if (nodeDistance > farthestNodeDistance) {
                        farthestNodeDistance = nodeDistance;
                        farthestNode = i;
                    }
                }
            }
        }

        // Step 2: Find the best insertion position for the farthest node
        double bestInsertionCost = __DBL_MAX__;
        int bestInsertPos = -1;
        for (int i = 0; i < tourLength - 1; i++) {
            double insertionCost = dMatrix[tour[i]][farthestNode] + dMatrix[farthestNode][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
            if (insertionCost < bestInsertionCost) {
                bestInsertionCost = insertionCost;
                bestInsertPos = i + 1;
            }
        }

        // Step 3: Insert the farthest node at the best position
        for (int i = tourLength; i > bestInsertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[bestInsertPos] = farthestNode;
        visited[farthestNode] = true;
        totalDistance += bestInsertionCost; // Add the cost of inserting the farthest node
        tourLength++;
        numVisited++;
    }

    free(visited);
    free(tour);

    // TourResult result;
    // result.tour = tour;
    // result.totalDistance = totalDistance;
    return tour;
}
