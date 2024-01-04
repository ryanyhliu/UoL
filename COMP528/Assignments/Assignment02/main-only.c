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

int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
double **createDistanceMatrix(double **coords, int numOfCoords);
double sqrt(double arg);
// int *cheapestInsertion(double **dMatrix, int numOfCoords);
TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *findCheapestTour(double **dMatrix, int numOfCoords);
int *findFarthestTour(double **dMatrix, int numOfCoords);
int *findNearestTour(double **dMatrix, int numOfCoords);

int main(int argc, char *argv[]){
	char filename[500];
	char outFileName_Cheapest[500];
	char outFileName_Farthest[500];
	char outFileName_Nearest[500];

	strcpy(filename, argv[1]);
	strcpy(outFileName_Cheapest, argv[2]);
	strcpy(outFileName_Farthest, argv[3]);
	strcpy(outFileName_Nearest, argv[4]);

	if(filename == NULL || outFileName_Cheapest == NULL || outFileName_Farthest == NULL || outFileName_Nearest == NULL){
		printf("Error reading files");
		return 1;
	}

	int numOfCoords = readNumOfCoords(filename);

	double **coords = readCoords(filename, numOfCoords);

	double tStart = omp_get_wtime();

	double **dMatrix = createDistanceMatrix(coords, numOfCoords);
	// int *tour = cheapestInsertion(dMatrix, numOfCoords);
	int *tourCheapest = findCheapestTour(dMatrix, numOfCoords);
	int *tourFarthest = findFarthestTour(dMatrix, numOfCoords);
	int *tourNearest = findNearestTour(dMatrix, numOfCoords);

	
	double tEnd = omp_get_wtime();

	printf("Took %f milliseconds\n", (tEnd - tStart) * 1000);

	if (writeTourToFile(tourCheapest, numOfCoords + 1, outFileName_Cheapest) == NULL){
		printf("Error");
	}
	if (writeTourToFile(tourFarthest, numOfCoords + 1, outFileName_Farthest) == NULL){
		printf("Error");
	}
	if (writeTourToFile(tourNearest, numOfCoords + 1, outFileName_Nearest) == NULL){
		printf("Error");
	}

	for(int i = 0; i < numOfCoords; i++){
		free(dMatrix[i]);
	}

	free(dMatrix);
	free(tourCheapest);
	free(tourFarthest);
	free(tourNearest);

}

int *findCheapestTour(double **dMatrix, int numOfCoords){
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    pathResult.totalDistance = 0.0;

    // 遍历起点
    int i = 0;
    for (int i = 0; i < numOfCoords; i++)
    {
        pathResult = cheapestInsertion(dMatrix, numOfCoords, i);
        if (pathResult.totalDistance < minDistance)
        {
            // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
            minDistance = pathResult.totalDistance;
            minTour = pathResult.tour;
        }        
    }
	// free(pathResult.tour);
    return minTour;
}

TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd){
	TourResult result;
    result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    result.totalDistance = 0.0;
	
	//Setting up variables
	int i, j;
	int nextNode, insertPos;

	//Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
	int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
	bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

	//Initialising tour to empty
	for(i = 0; i < numOfCoords; i++){
		tour[i] = -1;
	}

	//Tour always starts with 0. 0 is visited
	tour[0] = pointOfStartEnd;
	tour[1] = pointOfStartEnd;
	visited[pointOfStartEnd] = true;
	
	//Hard coding because I'm lazy
	int numVisited = 1;
	int tourLength = 2;
	
	//While there are still unvisited vertices
	while(numVisited < numOfCoords){
		double minCost = __DBL_MAX__;
		double thisDistance = 0.0;

		//Find an unvisited vertex such that the cost of adding it to the tour is minimal 
		for(i = 0; i < tourLength - 1; i++){
			for(j = 0; j < numOfCoords; j++){
				if(!visited[j]){
					double cost = dMatrix[tour[i]][j] + dMatrix[j][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
					if(cost < minCost){
						minCost = cost;
						nextNode = j;
						insertPos = i + 1;
						thisDistance = cost; // 增加的路径长度
					}
				}
			}
		}		

		//Shift array to add next vertex
		for(i = numOfCoords; i > insertPos; i--){
			tour[i] = tour[i - 1];
		}

		tour[insertPos] = nextNode;
		visited[nextNode] = true;
		
		tourLength++;
		numVisited++;
		result.totalDistance += thisDistance;
	}

	result.tour = tour;

	// free(tour);
	free(visited);

	return result;
}

int *findFarthestTour(double **dMatrix, int numOfCoords){
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    pathResult.totalDistance = 0.0;

    // 遍历起点
    int i = 0;
    for (int i = 0; i < numOfCoords; i++)
    {
        pathResult = farthestInsertion(dMatrix, numOfCoords, i);
        if (pathResult.totalDistance < minDistance)
        {
            // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
            minDistance = pathResult.totalDistance;
            minTour = pathResult.tour;
        }        
    }
	// free(pathResult.tour);
    return minTour;
}

TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd){
	TourResult result;
    result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    result.totalDistance = 0.0;

	//Setting up variables
	int i, j, k;
	int nextNode, insertPos;

	//Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
	int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
	bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

	//Initialising tour to empty
	for(i = 0; i < numOfCoords; i++){
		tour[i] = -1;
	}

	//Tour always starts with 0. 0 is visited
	tour[0] = pointOfStartEnd;
	tour[1] = pointOfStartEnd;
	visited[pointOfStartEnd] = true;
	
	//Hard coding because I'm lazy
	int numVisited = 1;
	int tourLength = 2;
	
	/*While there are still some unvisited vertices*/
	while(numVisited < numOfCoords){
		double maxCost = 0;
		double minCost = __DBL_MAX__;
		double thisDistance = 0.0;

		/*Find a vertex that is furthest from any vertex in the tour*/
		for(i = 0; i < tourLength - 1; i++){
			for(j = 0; j < numOfCoords; j++){
				if(!visited[j]){
					double cost = dMatrix[tour[i]][j];
					if(cost > maxCost){
						maxCost = cost;
						nextNode = j;
						
					}
				}
			}
		}
		
		/*Add that farthest vertex into a position in the tour such that the increased cost is minimum*/
		for(k = 0; k < tourLength - 1; k ++){
			double cost = dMatrix[tour[k]][nextNode] + dMatrix[nextNode][tour[k + 1]] - dMatrix[tour[k]][tour[k + 1]];
			if(cost < minCost){
				minCost = cost;
				insertPos = k + 1;
				thisDistance = cost; // 增加的路径长度
			}			
		}
		
		for(i = numOfCoords; i > insertPos; i--){
			tour[i] = tour[i - 1];
		}

		tour[insertPos] = nextNode;
		visited[nextNode] = true;
		tourLength++;
		result.totalDistance += thisDistance;

		numVisited++;
	}

	result.tour = tour;
	free(visited);

	return result;
}

int *findNearestTour(double **dMatrix, int numOfCoords){
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    pathResult.totalDistance = 0.0;

    // 遍历起点
    int i = 0;
    for (int i = 0; i < numOfCoords; i++)
    {
        pathResult = nearestAddition(dMatrix, numOfCoords, i);
        if (pathResult.totalDistance < minDistance)
        {
            // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
            minDistance = pathResult.totalDistance;
            minTour = pathResult.tour;

			// TODO	debug
			// printf("Tour: ");
			// for (int i = 0; i < numOfCoords + 1; i++)
			// {
			// 	printf(" %d", minTour[i]);
			// }
			// printf("\n");
        }        
    }

	
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

    while(numVisited < numOfCoords){
        double minDistance = __DBL_MAX__;
        double thisDistance = 0.0;
        
        // 寻找最近的未访问顶点
        for (i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                for (j = 1; j < tourLength; j++) { // 反正要比较最后一个, 不用比较第一个了
                    double distance = dMatrix[tour[j]][i];
                    if (distance < minDistance) {
                        minDistance = distance;
                        nearestNode = i;

                        // printf("---TEST 03: 当前轮 (%d -> %d) Dist: %.5f\n", tour[j], i, dMatrix[i][tour[j]]);

                        // 决定插入位置 (前/后), 选总长度增加最小的 -> 实际上就是比较 (j-1, i) or (i, j+1) 
                        if (j == tourLength - 1)
                        {
                            // 如果是终点
                            if (dMatrix[tour[0]][i] + dMatrix[i][tour[1]] - dMatrix[tour[0]][tour[1]] <= dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]])
                            {
                                insertPos = 1;
                                // printf("---TEST 03: 起点后 (%d -> %d) Dist: %.5f\n", 1, i, dMatrix[i][tour[1]]);
                                thisDistance = dMatrix[tour[0]][i] + dMatrix[i][tour[1]] - dMatrix[tour[0]][tour[1]];
                            }
                            else
                            {
                                insertPos = j;
                                // printf("---TEST 03: 终点前 (%d -> %d) Dist: %.5f\n", tour[j - 1], i, dMatrix[i][tour[j - 1]]);
                                thisDistance = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                            }
                        }
                        else // 比较插入点前边点 / 后边点 与添加点的新增距离
                        {
                            if (dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]] <= dMatrix[i][tour[j]] + dMatrix[i][tour[j + 1]] - dMatrix[tour[j]][tour[j + 1]])
                            {
                                insertPos = j;
                                // printf("---TEST 03: 点前 (%d -> %d) Dist: %.5f\n", tour[j - 1], i, dMatrix[i][tour[j - 1]]);
                                thisDistance = dMatrix[tour[j - 1]][i] + dMatrix[i][tour[j]] - dMatrix[tour[j - 1]][tour[j]];
                            }
                            else
                            {
                                insertPos = j + 1;
                                // printf("---TEST 03: 点后 (%d -> %d) Dist: %.5f\n", i, tour[j + 1], dMatrix[i][tour[j + 1]]);
                                thisDistance = dMatrix[i][tour[j]] + dMatrix[i][tour[j + 1]] - dMatrix[tour[j]][tour[j + 1]];
                            }
                        }
                        
                        // insertPos = (j == 0 || j == tourLength - 1) ? tourLength - 1 : j;
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


        // TODO debug
        // printf("---TEST 01: ");
        // for (i = 0; i < numOfCoords + 1; i++){
        //     printf(" %d", tour[i]);
        // }
        // printf("\n");
        // // printf("---TEST 02: Distance: %.5f", minDistance);
        // // printf("\n");

    }

    result.tour = tour;

    free(visited);
    // free(tour);

    return result;
}



double **createDistanceMatrix(double **coords, int numOfCoords){
	int i, j;
	
	double **dMatrix = (double **)malloc(numOfCoords * sizeof(double *));

	for(i = 0; i < numOfCoords; i++){
		dMatrix[i] = (double *)malloc(numOfCoords * sizeof(double));
	}

	for(i = 0; i < numOfCoords; i++){
		for(j = 0; j < numOfCoords; j++){
			double diffX = coords[i][0] - coords[j][0];
			double diffY = coords[i][1] - coords[j][1];
			dMatrix[i][j] = sqrt((diffX * diffX) + (diffY * diffY));
		}
	}

	return dMatrix;

}


