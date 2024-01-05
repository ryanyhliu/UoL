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



TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd){
    TourResult result;
    result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    result.totalDistance = 0.0;

    int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

    int i, j, nearestNode, insertPos;

    // 初始化路径和访问状态
    for (i = 0; i < numOfCoords; i++) {
        visited[i] = false;
    }

    tour[0] = pointOfStartEnd;
    tour[1] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;

    int numVisited = 1;
    int tourLength = 2;

    while(numVisited < numOfCoords){
        double minDistance = __DBL_MAX__;
        int nearestNode = -1;
        int insertPos = -1;
        double thisDistance = 0.0;

        #pragma omp parallel for private(j) shared(minDistance, nearestNode, insertPos, thisDistance)
        for (i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                for (j = 1; j < tourLength; j++) {
                    double distance = dMatrix[tour[j]][i];
                    if (distance < minDistance) {
                        #pragma omp critical
                        {
                            if (distance < minDistance) {
                                minDistance = distance;
                                nearestNode = i;

                                // 决定插入位置的逻辑
                                if (j == tourLength - 1) {
                                    // 如果是终点
                                    double distanceToEnd = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                                    double distanceToStart = dMatrix[tour[0]][i] + dMatrix[i][tour[1]] - dMatrix[tour[0]][tour[1]];
                                    if (distanceToStart <= distanceToEnd) {
                                        insertPos = 1;
                                        thisDistance = distanceToStart;
                                    } else {
                                        insertPos = j;
                                        thisDistance = distanceToEnd;
                                    }
                                } else {
                                    double distanceBefore = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                                    double distanceAfter = dMatrix[i][tour[j]] + dMatrix[i][tour[j + 1]] - dMatrix[tour[j]][tour[j + 1]];
                                    if (distanceBefore <= distanceAfter) {
                                        insertPos = j;
                                        thisDistance = distanceBefore;
                                    } else {
                                        insertPos = j + 1;
                                        thisDistance = distanceAfter;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // 更新路径和访问状态
        for (i = tourLength; i > insertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[insertPos] = nearestNode;
        visited[nearestNode] = true;
        numVisited++;
        tourLength++;
        result.totalDistance += thisDistance;
    }

    result.tour = tour;

    free(visited);
    // free(tour);

    return result;
}




// TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd){
//     TourResult result;
//     result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     result.totalDistance = 0.0;

//     int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
//     bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

//     int i, j, nearestNode, insertPos;

//     // 初始化路径和访问状态
//     for (i = 0; i < numOfCoords; i++) {
//         // tour[i] = -1;
//         visited[i] = false;
//     }
    

//     tour[0] = pointOfStartEnd; // 起点
//     tour[1] = pointOfStartEnd; // 初始时回到起点
//     visited[pointOfStartEnd] = true; // 标记起点为已访问

// 	int numVisited = 1;
//     int tourLength = 2; // 当前路径长度


// 	// OMP
// 	int numThreads = omp_get_max_threads();
//     double *threadMinDistances = (double *)malloc(numThreads * 8 * sizeof(double));
//     int *threadNearestNode = (int *)malloc(numThreads * 8 * sizeof(int));
//     int *threadInsertPos = (int *)malloc(numThreads * 8 * sizeof(int));

// 	#pragma omp parallel
// 	{
// 		int threadID = omp_get_thread_num();
		
// 		while(numVisited < numOfCoords){
// 			// double minDistance = __DBL_MAX__;
// 			// double thisDistance = 0.0;

// 			threadMinDistances[threadID * 8] = __DBL_MAX__;
//             threadNearestNode[threadID * 8] = -1;
//             threadInsertPos[threadID * 8] = -1;
			

// 			// 寻找最近的未访问顶点
// 			#pragma omp for nowait
// 			for (i = 0; i < numOfCoords; i++) {
// 				if (!visited[i]) {
// 					for (j = 1; j < tourLength; j++) { // 反正要比较终点, 不用比较起点了
// 						double distance = dMatrix[tour[j]][i];

// 						if (distance < threadMinDistances[threadID * 8])
// 						{
// 							threadMinDistances[threadID * 8] = distance;
// 							threadNearestNode[threadID * 8] = i;

// 							double addDistance;
// 							int insertPos;
							
// 							if (j == tourLength - 1)
// 							{
// 								double leftDistance = dMatrix[tour[0]][i] + dMatrix[i][tour[1]] - dMatrix[tour[0]][tour[1]];
// 								double rightDistance = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];

// 								if (leftDistance <= rightDistance)
// 								{
// 									addDistance = leftDistance;
// 									insertPos = 1;
// 								}
// 								else
// 								{
// 									addDistance = rightDistance;
// 									insertPos = j;
// 								}
// 							}
// 							else
// 							{
// 								double leftDistance = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
// 								double rightDistance = dMatrix[i][tour[j]] + dMatrix[i][tour[j + 1]] - dMatrix[tour[j]][tour[j + 1]];

// 								if (leftDistance <= rightDistance)
// 								{
// 									addDistance = leftDistance;
// 									insertPos = j;
// 								}
// 								else
// 								{
// 									addDistance = rightDistance;
// 									insertPos = j + 1;
// 								}
// 							}
// 							threadInsertPos[threadID * 8] = insertPos;
// 						}
// 					}
// 				}
// 			}

// 			#pragma omp single
//             {
//                 double globalMinDistance = __DBL_MAX__;
//                 int globalNearestNode = -1;
//                 int globalInsertPos = -1;
//                 for(int i = 0; i < numThreads; i++){
//                     if(threadMinDistances[i * 8] < globalMinDistance){
//                         globalMinDistance = threadMinDistances[i * 8];
//                         globalNearestNode = threadNearestNode[i * 8];
//                         globalInsertPos = threadInsertPos[i * 8];
//                     }
//                 }

//                 if(globalNearestNode != -1){
//                     for(int i = tourLength; i > globalInsertPos; i--){
//                         tour[i] = tour[i - 1];
//                     }
//                     tour[globalInsertPos] = globalNearestNode;
//                     visited[globalNearestNode] = true;
//                     tourLength++;
//                     numVisited++;
//                     result.totalDistance += globalMinDistance;
//                 }
//             }


// 			// TODO debug
// 			// printf("---TEST 01: ");
// 			// for (i = 0; i < numOfCoords + 1; i++){
// 			//     printf(" %d", tour[i]);
// 			// }
// 			// printf("\n");
// 			// // printf("---TEST 02: Distance: %.5f", minDistance);
// 			// // printf("\n");

// 		}
// 	}
    

//     result.tour = tour;

//     free(threadMinDistances);
//     free(threadNearestNode);
//     free(threadInsertPos);
//     free(visited);

//     return result;
// }

