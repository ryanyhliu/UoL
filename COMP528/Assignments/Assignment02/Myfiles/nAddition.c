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
// int *nearestAddition(double **dMatrix, int numOfCoords);
TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *findTour(double **dMatrix, int numOfCoords);



int main(int argc, char *argv[]){
	char filename[500];
	char outFileName[500];

	strcpy(filename, argv[1]);
	strcpy(outFileName, argv[2]);

	if(filename == NULL || outFileName == NULL){
		printf("Error reading files");
		return 1;
	}

	int numOfCoords = readNumOfCoords(filename);

	double **coords = readCoords(filename, numOfCoords);

	double tStart = omp_get_wtime();

	double **dMatrix = createDistanceMatrix(coords, numOfCoords);


	int *tour = findTour(dMatrix, numOfCoords);

	
	double tEnd = omp_get_wtime();

	printf("\nTook %f milliseconds", (tEnd - tStart) * 1000);

	if (writeTourToFile(tour, numOfCoords + 1, outFileName) == NULL){
		printf("Error");
	}

	for(int i = 0; i < numOfCoords; i++){
		free(dMatrix[i]);
	}

	free(dMatrix);
	free(tour);

}

int *findTour(double **dMatrix, int numOfCoords){
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    pathResult.totalDistance = 0.0;
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    double minDistance = __DBL_MAX__;

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

    // TODO debug printMatrix
    for(i = 0; i < numOfCoords; i++){
        for(j = 0; j < numOfCoords; j++){
            printf(" %010.5f", dMatrix[i][j]);
        }
        printf("\n");
    }

	return dMatrix;

}


