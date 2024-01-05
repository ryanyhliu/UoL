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






TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd){
	TourResult result;
    result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    result.totalDistance = 0.0;

	//Setting up variables
	int nextNode, insertPos;

	//Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
	//Visited uses calloc, array is instantiated with "0" as all elements. Good for boolean arrays.
	int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
	bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

	if(tour == NULL){
		printf("Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	//Initialising tour to empty
	for(int i = 0; i < numOfCoords; i++){
		tour[i] = -1;
	}

	//Tour always starts and ends at 0. 0 is visited
	tour[0] = pointOfStartEnd;
	tour[1] = pointOfStartEnd;
	visited[pointOfStartEnd] = true;
	
	//Hard coding because I'm lazy
	int numVisited = 1;
	int tourLength = 2;

	//Where OMP starts... Get the env variable for the max num of threads.
	int numThreads = omp_get_max_threads();

	// printf("This program uses %d threads \n\n", numThreads);
	
	/*
	Set up arrays to be the size of the number of threads. Each thread will store 
	its minCost, its nextNode, and its insertPos in its respective memory location.
	Thread 0 will store its results at position 0, thread 1 will store its results at position 1 etc.
	*/

	double *threadMinCosts = NULL;
	int *threadNextNode = NULL;
	int *threadInsertPos = NULL;
		
	threadMinCosts = (double*)malloc(numThreads * 8 * sizeof(double));
	threadNextNode = (int*)malloc(numThreads * 8 * sizeof(int));
	threadInsertPos = (int*)malloc(numThreads * 8 * sizeof(int));

	//Start a parallel section
	#pragma omp parallel 
	{
		//Each thread now has started, and it stores its thread number in threadID
		int threadID = omp_get_thread_num();
		while(numVisited < numOfCoords){

			//Thread only accesses its memory location in the shared array. No race conditions.
			threadMinCosts[threadID * 8] = __DBL_MAX__;
			threadNextNode[threadID * 8] = -1;
			threadInsertPos[threadID * 8] = -1;

			//Begin a workshare construct. Threads divide i and j and work on their respective iterations.
			#pragma omp for collapse(2)
			for(int i = 0; i < tourLength - 1; i++){	
				for(int j = 0; j < numOfCoords; j++){

					//Each thread performs their cheapest insertion. Works on each position in the tour.
					if(!visited[j]){
						double cost = dMatrix[tour[i]][j] + dMatrix[tour[i+1]][j] - dMatrix[tour[i]][tour[i + 1]];
						if(cost < threadMinCosts[threadID * 8]){

							threadMinCosts[threadID * 8] = cost;
							threadNextNode[threadID * 8] = j;
							threadInsertPos[threadID * 8] = i + 1;
						}
					}
				}
			}

			//Only one thread works on this part. This part must be serial. OMP single instead of master. Therefore implicit barrier
			#pragma omp single
			{
				int bestNextNode = -1;
				int bestInsertPos = -1;
				double minCost = __DBL_MAX__;

				//A single thread loops through each threads memory locations. Finds the minCost
				for(int i = 0; i < numThreads; i++){
					if(threadMinCosts[i * 8] < minCost){
						minCost = threadMinCosts[i * 8];
						bestNextNode = threadNextNode[i * 8];
						bestInsertPos = threadInsertPos[i * 8];

						result.totalDistance += minCost;
					}
				}	

				//One thread places the bestNextNode in the bestInsertPos
				for(int i = numOfCoords; i > bestInsertPos; i--){
					tour[i] = tour[i - 1];
				}

				tour[bestInsertPos] = bestNextNode;
				visited[bestNextNode] = true;		
				
				tourLength++;
				numVisited++;

			}
		}
	}

	
	result.tour = tour;

	//Free all memory when done
	free(visited);
	free(threadMinCosts);
	free(threadNextNode);
	free(threadInsertPos);

	return result;
}

