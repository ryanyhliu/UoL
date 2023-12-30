#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<omp.h>
#include<string.h>

int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
double **createDistanceMatrix(double **coords, int numOfCoords);
double sqrt(double arg);
int *cheapestInsertion(double **dMatrix, int numOfCoords);

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
	int *tour = cheapestInsertion(dMatrix, numOfCoords);

	
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

int *nearestAddition(double **dMatrix, int numOfCoords){
    // 设置变量
    int i, j;
    int nextNode, insertPos;

    // 为路径和已访问数组分配内存。路径长度为numOfCoords + 1，为了返回起点
    int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
    bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

    // 初始化路径为未定义
    for(i = 0; i < numOfCoords; i++){
        tour[i] = -1;
    }

    // 路径始终从0开始。0已被访问
    tour[0] = 0;
    tour[1] = 0;
    visited[0] = true;
    
    // 由于懒惰，硬编码
    int numVisited = 1;
    int tourLength = 2;
    
    // 当还有未访问的顶点时
    while(numVisited < numOfCoords){
        double minCost = __DBL_MAX__;

        // 寻找最近的未访问顶点
        for(i = 0; i < numOfCoords; i++){
            if(!visited[i]){
                double cost = dMatrix[tour[tourLength - 1]][i];
                if(cost < minCost){
                    minCost = cost;
                    nextNode = i;
                }
            }
        }

        // 将最近的顶点添加到路径末尾
        tour[tourLength] = nextNode;
        visited[nextNode] = true;
        tourLength++;
        numVisited++;
    }

    // 释放已访问数组的内存
    free(visited);
    return tour;
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


