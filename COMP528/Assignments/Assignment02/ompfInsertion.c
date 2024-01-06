#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include<omp.h>


typedef struct {
    int *tour;
    double totalDistance;
} TourResult;





// TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
//     int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
//     bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

// 	#pragma omp parallel for
//     for(int i = 0; i < numOfCoords; i++) {
//         tour[i] = -1;
//     }

//     tour[0] = pointOfStartEnd;
//     tour[1] = pointOfStartEnd;
//     visited[pointOfStartEnd] = true;

//     int numVisited = 1;
//     int tourLength = 2;
//     double totalDistance = 0.0;

//     int numThreads = omp_get_max_threads();
//     double *threadMaxCosts = (double *)malloc(numThreads * sizeof(double));
//     int *threadNextNode = (int *)malloc(numThreads * sizeof(int));

//     while(numVisited < numOfCoords) {
//         int bestNextNode = -1;
//         double maxCost = 0;

//         #pragma omp parallel 
//         {
//             int threadID = omp_get_thread_num();
//             threadMaxCosts[threadID] = 0;
//             threadNextNode[threadID] = -1;

//             #pragma omp for nowait
//             for(int i = 0; i < tourLength - 1; i++) {
//                 for(int j = 0; j < numOfCoords; j++) {
//                     if(!visited[j]) {
//                         double cost = dMatrix[tour[i]][j];
//                         if(cost > threadMaxCosts[threadID]) {
//                             threadMaxCosts[threadID] = cost;
//                             threadNextNode[threadID] = j;
//                         }
//                     }
//                 }
//             }

//             #pragma omp critical
//             {
//                 if (threadMaxCosts[threadID] > maxCost) {
//                     maxCost = threadMaxCosts[threadID];
//                     bestNextNode = threadNextNode[threadID];
//                 }
//             }
//         }

//         double minCost = __DBL_MAX__;
//         int insertPos = -1;

// 		// #pragma omp parallel for
//         for(int k = 0; k < tourLength - 1; k++) {
//             double cost = dMatrix[tour[k]][bestNextNode] + dMatrix[bestNextNode][tour[k + 1]] - dMatrix[tour[k]][tour[k + 1]];
//             if(cost < minCost) {
//                 minCost = cost;
//                 insertPos = k + 1;
//             }
//         }

// 		// #pragma omp parallel for
//         for(int i = numOfCoords; i > insertPos; i--) {
//             tour[i] = tour[i - 1];
//         }

//         tour[insertPos] = bestNextNode;
//         visited[bestNextNode] = true;
//         totalDistance += minCost;
//         tourLength++;
//         numVisited++;
//     }

//     free(threadMaxCosts);
//     free(threadNextNode);
//     free(visited);

//     TourResult result;
//     result.tour = tour;
//     result.totalDistance = totalDistance;
//     return result;
// }




TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
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

    while(numVisited < numOfCoords) {
        int bestNextNode = -1;
        double globalMaxCost = 0;

        #pragma omp parallel 
        {
            int threadID = omp_get_thread_num();
            maxCosts[threadID] = 0;
            nextNodes[threadID] = -1;

            #pragma omp for nowait
            for(int i = 0; i < tourLength - 1; i++) {
                for(int j = 0; j < numOfCoords; j++) {
                    if(!visited[j]) {
                        double cost = dMatrix[tour[i]][j];
                        if(cost > maxCosts[threadID]) {
                            maxCosts[threadID] = cost;
                            nextNodes[threadID] = j;
                        }
                    }
                }
            }

            // 在每个线程结束后执行，无需 critical
            #pragma omp flush (globalMaxCost)
            if (maxCosts[threadID] > globalMaxCost) {
                #pragma omp critical
                {
                    if (maxCosts[threadID] > globalMaxCost) {
                        globalMaxCost = maxCosts[threadID];
                        bestNextNode = nextNodes[threadID];
                    }
                }
            }
        }

        double minCost = __DBL_MAX__;
        int insertPos = -1;

        // 不是并行的部分
        for(int k = 0; k < tourLength - 1; k++) {
            double cost = dMatrix[tour[k]][bestNextNode] + dMatrix[bestNextNode][tour[k + 1]] - dMatrix[tour[k]][tour[k + 1]];
            if(cost < minCost - tolerance) {
                minCost = cost;
                insertPos = k + 1;
            }
        }

        for(int i = numOfCoords; i > insertPos; i--) {
            tour[i] = tour[i - 1];
        }

        tour[insertPos] = bestNextNode;
        visited[bestNextNode] = true;
        totalDistance += minCost;
        tourLength++;
        numVisited++;
    }

    free(maxCosts);
    free(nextNodes);
    free(visited);

    TourResult result;
    result.tour = tour;
    result.totalDistance = totalDistance;
    return result;
}




// TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd){
// 	TourResult result;
//     result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     result.totalDistance = 0.0;
	

// 	//Setting up variables
// 	int nextNode, insertPos;

// 	//Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
// 	//Visited uses calloc, array is instantiated with "0" as all elements. Good for boolean arrays.
// 	int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
// 	bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

// 	//Initialising tour to empty
// 	for(int i = 0; i < numOfCoords; i++){
// 		tour[i] = -1;
// 	}

// 	//Tour always starts with 0. 0 is visited
// 	tour[0] = pointOfStartEnd;
// 	tour[1] = pointOfStartEnd;
// 	visited[pointOfStartEnd] = true;
	
// 	//Hard coding because I'm lazy
// 	int numVisited = 1;
// 	int tourLength = 2;

// 	//Where OMP starts... Get the env variable for the max num of threads.
// 	int numThreads = omp_get_max_threads();
	
// 	/*
// 	Set up arrays to be the size of the number of threads. Each thread will store 
// 	its minCost, its nextNode, and its insertPos in its respective memory location.
// 	Thread 0 will store its results at position 0, thread 1 will store its results at position 1 etc.
// 	Multiply by 8 to avoid false sharing. Each space is 64 bytes long (to ensure each thread has its own cache line)
// 	*/

// 	double *threadMinCosts = NULL;
// 	double *threadMaxCosts = NULL;
// 	int *threadNextNode = NULL;
// 	int *threadInsertPos = NULL;
		
// 	threadMinCosts = (double*)malloc(numThreads * 8 * sizeof(double));
// 	threadMaxCosts = (double*)malloc(numThreads * 8 * sizeof(double));
// 	threadNextNode = (int*)malloc(numThreads * 8 *sizeof(int));
// 	threadInsertPos = (int*)malloc(numThreads * 8 *sizeof(int));
	
// 	int bestNextNode = -1;

// 	//Start a parallel section
// 	#pragma omp parallel 
// 	{

// 		//Each thread now has started, and it stores its thread number in threadID
// 		int threadID = omp_get_thread_num();
		
// 		while(numVisited < numOfCoords){

// 			//Point 1: Thread only accesses its memory location in the shared array.
// 			threadMinCosts[threadID * 8] = __DBL_MAX__;
// 			threadMaxCosts[threadID * 8] = 0;
// 			threadNextNode[threadID * 8] = -1;
// 			threadInsertPos[threadID * 8] = -1;

// 			//Begin a workshare construct. Threads divide i and j and work on their respective ones.
// 			#pragma omp for collapse(2)
// 			for(int i = 0; i < tourLength - 1; i++){
// 				for(int j = 0; j < numOfCoords; j++){
// 					//Each thread identifies their farthest vertex from a vertex in the tour
// 					if(!visited[j]){
// 						double cost = dMatrix[tour[i]][j];
// 						if(cost > threadMaxCosts[threadID * 8]){
// 							//See Point 1
// 							threadMaxCosts[threadID * 8] = cost;
// 							threadNextNode[threadID * 8] = j;
// 						}
// 					}
// 				}
// 			}

// 			//Single construct, one thread looks through what each thread has. Choosest the farthest node.
// 			#pragma omp single
// 			{
// 				double maxCost = 0;
// 				for(int i = 0; i < numThreads; i++){
// 					if(threadMaxCosts[i * 8] > maxCost){
// 						maxCost = threadMaxCosts[i * 8];
// 						bestNextNode = threadNextNode[i * 8];
// 					}
// 				}

				
// 			}

// 			//Find the cost of adding the farthest node to every possible location in the tour. Each thread finds their own.
// 			#pragma omp for
// 			for(int k = 0; k < tourLength - 1; k++){
// 				double cost = dMatrix[tour[k]][bestNextNode] + dMatrix[bestNextNode][tour[k + 1]] - dMatrix[tour[k]][tour[k + 1]];
// 				if(cost < threadMinCosts[threadID * 8]){
// 					threadMinCosts[threadID * 8] = cost;
// 					threadInsertPos[threadID * 8] = k + 1;
// 				}
// 			}

// 			//Single construct only one thread working on this part.
// 			#pragma omp single
// 			{
// 			int bestInsertPos = -1;
// 			double minCost = __DBL_MAX__;

// 			//Single thread loops through every thread's answer and chooses the cheapest one.
// 			for(int i = 0; i < numThreads; i++){
// 				if(threadMinCosts[i * 8] < minCost){
// 					minCost = threadMinCosts[i * 8];
// 					bestInsertPos = threadInsertPos[i * 8];
// 				}
// 			}	

// 			//Single thread places the bestNextNode in the bestInsertPos
// 			for(int i = numOfCoords; i > bestInsertPos; i--){
// 				tour[i] = tour[i - 1];
// 			}

// 			tour[bestInsertPos] = bestNextNode;
// 			visited[bestNextNode] = true;		
			
// 			tourLength++;
// 			numVisited++;
// 			result.totalDistance += minCost;

// 			}
// 		}
// 	}

// 	//Free all memory when done


// 	result.tour = tour;

// 	free(visited);
// 	free(threadMinCosts);
// 	free(threadNextNode);
// 	free(threadInsertPos);
// 	free(threadMaxCosts);

// 	return result;
// }

