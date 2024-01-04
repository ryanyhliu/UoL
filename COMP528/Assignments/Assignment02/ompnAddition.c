#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<omp.h>
#include<string.h>

typedef struct {
    int *tour;
    double totalDistance;
} TourResult;



int *findNearestTour(double **dMatrix, int numOfCoords){
    // int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    // double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    pathResult.totalDistance = 0.0;

	// OMP
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
			TourResult pathResult = nearestAddition(dMatrix, numOfCoords, i);
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
    // int i = 0;
    // for (int i = 0; i < numOfCoords; i++)
    // {
    //     pathResult = nearestAddition(dMatrix, numOfCoords, i);
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

TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd){
    TourResult result;
    result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    result.totalDistance = 0.0;

    int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

    int i, j, nearestNode, insertPos;

    // 初始化路径和访问状态
    for (i = 0; i < numOfCoords; i++) {
        // tour[i] = -1;
        visited[i] = false;
    }
    

    tour[0] = pointOfStartEnd; // 起点
    tour[1] = pointOfStartEnd; // 初始时回到起点
    visited[pointOfStartEnd] = true; // 标记起点为已访问

	int numVisited = 1;
    int tourLength = 2; // 当前路径长度


	// OMP
	int numThreads = omp_get_max_threads();
    double *threadMinDistances = (double *)malloc(numThreads * 8 * sizeof(double));
    int *threadNearestNode = (int *)malloc(numThreads * 8 * sizeof(int));
    int *threadInsertPos = (int *)malloc(numThreads * 8 * sizeof(int));

	#pragma omp parallel
	{
		int threadID = omp_get_thread_num();
		
		while(numVisited < numOfCoords){
			// double minDistance = __DBL_MAX__;
			// double thisDistance = 0.0;

			threadMinDistances[threadID * 8] = __DBL_MAX__;
            threadNearestNode[threadID * 8] = -1;
            threadInsertPos[threadID * 8] = -1;
			

			// 寻找最近的未访问顶点
			#pragma omp for nowait
			for (i = 0; i < numOfCoords; i++) {
				if (!visited[i]) {
					for (j = 1; j < tourLength; j++) { // 反正要比较终点, 不用比较起点了
						double distance = dMatrix[tour[j]][i];

						if (distance < threadMinDistances[threadID * 8])
						{
							threadMinDistances[threadID * 8] = distance;
							threadNearestNode[threadID * 8] = i;

							double addDistance;
							int insertPos;
							
							if (j == tourLength - 1)
							{
								double leftDistance = dMatrix[tour[0]][i] + dMatrix[i][tour[1]] - dMatrix[tour[0]][tour[1]];
								double rightDistance = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];

								if (leftDistance <= rightDistance)
								{
									addDistance = leftDistance;
									insertPos = 1;
								}
								else
								{
									addDistance = rightDistance;
									insertPos = j;
								}
							}
							else
							{
								double leftDistance = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
								double rightDistance = dMatrix[i][tour[j]] + dMatrix[i][tour[j + 1]] - dMatrix[tour[j]][tour[j + 1]];

								if (leftDistance <= rightDistance)
								{
									addDistance = leftDistance;
									insertPos = j;
								}
								else
								{
									addDistance = rightDistance;
									insertPos = j + 1;
								}
							}
							threadInsertPos[threadID * 8] = pos;
						}
					}
				}
			}

			#pragma omp single
            {
                double globalMinDistance = __DBL_MAX__;
                int globalNearestNode = -1;
                int globalInsertPos = -1;
                for(int i = 0; i < numThreads; i++){
                    if(threadMinDistances[i * 8] < globalMinDistance){
                        globalMinDistance = threadMinDistances[i * 8];
                        globalNearestNode = threadNearestNode[i * 8];
                        globalInsertPos = threadInsertPos[i * 8];
                    }
                }

                if(globalNearestNode != -1){
                    for(int i = tourLength; i > globalInsertPos; i--){
                        tour[i] = tour[i - 1];
                    }
                    tour[globalInsertPos] = globalNearestNode;
                    visited[globalNearestNode] = true;
                    tourLength++;
                    numVisited++;
                    result.totalDistance += globalMinDistance;
                }
            }


			// TODO debug
			// printf("---TEST 01: ");
			// for (i = 0; i < numOfCoords + 1; i++){
			//     printf(" %d", tour[i]);
			// }
			// printf("\n");
			// // printf("---TEST 02: Distance: %.5f", minDistance);
			// // printf("\n");

		}
	}
    

    result.tour = tour;

    free(threadMinDistances);
    free(threadNearestNode);
    free(threadInsertPos);
    free(visited);

    return result;
}

