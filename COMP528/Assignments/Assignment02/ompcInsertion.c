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

int *findCheapestTour(double **dMatrix, int numOfCoords){
    // int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    // double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    pathResult.totalDistance = 0.0;

	// OMP
	int numThreads = omp_get_max_threads();
	double *threadMinCosts = (double *)malloc(numThreads * sizeof(double));
	int **threadMinTour = (int **)malloc(numThreads * sizeof(int *));
	double *threadMinDistance = (double *)malloc(numThreads * sizeof(double));
	for (int i = 0; i < numThreads; i++) {
		threadMinDistance[i] = __DBL_MAX__;
		threadMinTour[i] = (int *)malloc((numOfCoords + 1) * sizeof(int));
	}


	#pragma omp parallel
	{
		int threadID = omp_get_thread_num();
		threadMinCosts[threadID] = __DBL_MAX__;

		#pragma omp for
		for (int i = 0; i < numOfCoords; i++){
			TourResult pathResult = cheapestInsertion(dMatrix, numOfCoords, i);
			if (pathResult.totalDistance < threadMinDistance[threadID]) {
				threadMinDistance[threadID] = pathResult.totalDistance;
				memcpy(threadMinTour[threadID], pathResult.tour, (numOfCoords + 1) * sizeof(int));
			}
		}
	}

	// Find the overall min tour and distance
	int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
	double minDistance = __DBL_MAX__;
	for (int i = 0; i < numThreads; i++) {
		if (threadMinDistance[i] < minDistance) {
			minDistance = threadMinDistance[i];
			memcpy(minTour, threadMinTour[i], (numOfCoords + 1) * sizeof(int));
		}
		free(threadMinTour[i]); // 释放线程局部分配的内存
	}

	free(threadMinCosts);
    free(threadMinTour);
    free(threadMinDistance);


    // 遍历起点
    // for (i = 0; i < numOfCoords; i++)
    // {
    //     pathResult = cheapestInsertion(dMatrix, numOfCoords, i);
    //     if (pathResult.totalDistance < minDistance)
    //     {
    //         // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
    //         minDistance = pathResult.totalDistance;
    //         minTour = pathResult.tour;
    //     }        
    // }


	// free(pathResult.tour);


    return minTour;
}

TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd){
	TourResult result;
    result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    result.totalDistance = 0.0;
	
	//Setting up variables
	int i, j;

	//Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
	int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
	bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

	//Tour always starts with 0. 0 is visited
	tour[0] = pointOfStartEnd;
	tour[1] = pointOfStartEnd;
	visited[pointOfStartEnd] = true;
	
	//Hard coding because I'm lazy
	int numVisited = 1;
	int tourLength = 2;

	// OMP
	int numThreads = omp_get_max_threads();
    double *threadMinCosts = (double *)malloc(numThreads * 8 * sizeof(double));
    int *threadNextNode = (int *)malloc(numThreads * 8 * sizeof(int));
    int *threadInsertPos = (int *)malloc(numThreads * 8 * sizeof(int));


	

	#pragma omp parallel
	{
		int threadID = omp_get_thread_num();
		while (numVisited < numOfCoords) {
			threadMinCosts[threadID * 8] = __DBL_MAX__;  // 使用 threadID * 8
			threadNextNode[threadID * 8] = -1;
			threadInsertPos[threadID * 8] = -1;

			#pragma omp for collapse(2)
			for (int i = 0; i < tourLength - 1; i++) {
				for (int j = 0; j < numOfCoords; j++) {
					if (!visited[j]) {
						double cost = dMatrix[tour[i]][j] + dMatrix[j][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
						if (cost < threadMinCosts[threadID * 8]) {  // 使用 threadID * 8
							threadMinCosts[threadID * 8] = cost;
							threadNextNode[threadID * 8] = j;
							threadInsertPos[threadID * 8] = i + 1;
						}
					}
				}
			}
		

			#pragma omp single
			{
				int bestNextNode = -1;
				int bestInsertPos = -1;
				double minCost = __DBL_MAX__;

				for (int i = 0; i < numThreads; i++) {
					if (threadMinCosts[i * 8] < minCost) {
						minCost = threadMinCosts[i * 8];
						bestNextNode = threadNextNode[i * 8];
						bestInsertPos = threadInsertPos[i * 8];
					}
				}
			
				if (bestNextNode != -1) {
					for (int i = numOfCoords; i > bestInsertPos; i--) {
						tour[i] = tour[i - 1];
					}

					tour[bestInsertPos] = bestNextNode;
					visited[bestNextNode] = true;

					tourLength++;
					numVisited++;

					result.totalDistance += minCost;
				}
				else
				{
					printf("Error: GetBestNextNode\n");
				}
			}
		}
	}

	result.tour = tour;

	free(threadMinCosts);
    free(threadNextNode);
    free(threadInsertPos);
    free(visited);

	return result;
}