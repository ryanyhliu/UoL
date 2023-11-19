#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#include <time.h>

/*This code is for reading and writing to files for the 2023-24 COMP528 CA1*/

/*Use the functions in this file to read from the input file, and write to the output file*/

/*You should use this file when compiling your code*/

/*Declare these functions at the top of each 'main' file*/

/*If there are any issues with this code, please contact: h.j.forbes@liverpool.ac.uk*/

int readNumOfCoords(char *fileName);
double **readCoords(char *filename, int numOfCoords);

/*Gets the number of the coordinates in the file. Returns as a single integer*/
int readNumOfCoords(char *filename){
	FILE *file = fopen(filename, "r");
	int numOfCoords = 0;

	if(file == NULL){
		return -1;
	}

	char line[100];

	while(fgets(line, sizeof(line), file) != NULL){
		numOfCoords++;
	}
	
    return numOfCoords;
}

/*Gets the data from the file. Returns as an array of doubles, Ignores the first integer*/
double **readCoords(char *filename, int numOfCoords){
	FILE *file = fopen(filename,"r");
    int i;

	char line[100];
    
    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return NULL;
    }

	double **coords = (double **)malloc(numOfCoords * sizeof(double *));

	for(i = 0; i < numOfCoords; i++){
		coords[i] = (double *) malloc(2 * sizeof(int));
		if (coords[i] == NULL){
			perror("Memory Allocation Failed");
		}
	}

	int lineNum = 0;
	while(fgets(line, sizeof(line), file) != NULL){
		double x, y;
		if (sscanf(line, "%lf,%lf", &x, &y) == 2){
			coords[lineNum][0] = x;
			coords[lineNum][1] = y;
			lineNum++;
		}
	}

	return coords;
}

void *writeTourToFile(int *tour, int tourLength, char *filename){
	
	FILE *file = fopen(filename, "w");
	int i;	
	
	if(file == NULL){
		printf("Unable to open file: %s", filename);
		return NULL;
	}

	fprintf(file, "%d \n", tourLength);

	printf("Writing output data\n");
    for(i=0; i < tourLength; i++) {
        fprintf(file, "%d ", tour[i]);
    }

}

////////////////// YH's Code Begin



typedef struct
{
	int insertPoint; // 要插入的点
	int insertIndex; // 插入的位置
	double cost;	 // 插入的成本
} InsertionTask;

double const DBL_MIN = 0.0;
double const DBL_MAX = 99999.99999;



InsertionTask findFarthestInsertion(int *seq, double **dist, int numOfCoords, int seqValidLen) {
    InsertionTask globalBestTask = {-1, -1, -1.0};

    // 初始化已在序列中的点的集合
    bool *inSeq = (bool *)malloc(numOfCoords * sizeof(bool));
    for (int i = 0; i < numOfCoords; i++) {
        inSeq[i] = false;
    }
    for (int i = 0; i < seqValidLen; i++) {
        inSeq[seq[i]] = true;
    }

    // 寻找最远点
#pragma omp parallel
    {
        InsertionTask localBestTask = {-1, -1, -1.0};

#pragma omp for nowait
        for (int i = 0; i < seqValidLen; i++) {
            for (int j = 0; j < numOfCoords; j++) {
                if (!inSeq[j]) {
                    double currentDist = dist[seq[i]][j];
                    if (currentDist > localBestTask.cost) {
                        localBestTask.insertPoint = j;
                        localBestTask.cost = currentDist;
                        localBestTask.insertIndex = i + 1; // 这里预设一个插入位置
                    }
                }
            }
        }

#pragma omp critical
        {
            if (localBestTask.cost > globalBestTask.cost) {
                globalBestTask = localBestTask;
            }
        }
    }

    free(inSeq); // 释放集合内存

    // 重新计算最佳插入位置
    double tempClosest = DBL_MAX;
    for (int i = 0; i < seqValidLen - 1; i++) {
        int j = i + 1;
        double insertCost = dist[seq[i]][globalBestTask.insertPoint] + dist[globalBestTask.insertPoint][seq[j]] - dist[seq[i]][seq[j]];
        if (insertCost < tempClosest) {
            tempClosest = insertCost;
            globalBestTask.insertIndex = i + 1;
        }
    }

    return globalBestTask;
}











void insertPoint(int *seq, int seqLen, InsertionTask task) {
    if (task.insertIndex < seqLen + 1) {
        for (int i = seqLen; i >= task.insertIndex; i--) {
            seq[i + 1] = seq[i];
        }
        seq[task.insertIndex] = task.insertPoint;
    }
}








double euclideanDistance(double *a, double *b)
{
	return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

void calculateDist(double **inputs, double **dist, int numOfCoords)
{
#pragma omp parallel for
	for (int i = 0; i < numOfCoords; i++)
	{
#pragma omp parallel for
		for (int j = 0; j < numOfCoords; j++)
		{
			if (i != j)
			{
				dist[i][j] = euclideanDistance(inputs[i], inputs[j]);
			}
			else
			{
				dist[i][j] = 0.0;
			}
		}
	}
}



int main(int argc, char *argv[]) {
    clock_t startTime = clock();

    // 初始化和读取输入数据
    char *inputFileName = "16_coords.coord"; // 根据实际情况调整文件名
    int numOfCoords = readNumOfCoords(inputFileName);
    double **inputs = readCoords(inputFileName, numOfCoords);

    // 初始化距离矩阵
    double **dist = (double **)malloc(numOfCoords * sizeof(double *));
    for (int i = 0; i < numOfCoords; i++) {
        dist[i] = (double *)malloc(numOfCoords * sizeof(double));
    }
    calculateDist(inputs, dist, numOfCoords);

    // 初始化序列，包括起始点
    int *resultSeq = (int *)malloc((numOfCoords + 1) * sizeof(int));
    resultSeq[0] = 0;
    for (int i = 1; i <= numOfCoords; i++) {
        resultSeq[i] = -1;
    }

    int currentSeqLen = 1;

	for (int i = 0; i < numOfCoords; i++){
		InsertionTask task = findFarthestInsertion(resultSeq, dist, numOfCoords, currentSeqLen);
		if (task.insertPoint != -1){
			insertPoint(resultSeq, currentSeqLen, task);
			currentSeqLen++;
		}

		printf("Current sequence: ");
        for (int j = 0; j < currentSeqLen; j++) {
            printf("%d ", resultSeq[j]);
        }
        printf("\n");
	}

	// 添加结束点
	resultSeq[numOfCoords] = 0;

    // 输出最终序列
    printf("\nFinal sequence: ");
    for (int i = 0; i < numOfCoords + 1; i++) {
        printf("%d ", resultSeq[i]);
    }

    // 清理资源
    for (int i = 0; i < numOfCoords; i++) {
        free(dist[i]);
    }
    free(dist);
    free(inputs);
    free(resultSeq);

    printf("\nTIME: %f", ((double)(clock() - startTime) / CLOCKS_PER_SEC));
    return 0;
}