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




typedef struct {
    int farthestPoint;  // 最远点
    int insertIndex;    // 插入位置
    double distance;    // 到最远点的距离
} FarthestInsertionTask;

double const DBL_MAX = 99999.99999;



FarthestInsertionTask findFarthestInsertion(int *seq, double **dist, int numOfCoords, int seqValidLen) {
    FarthestInsertionTask globalBestTask = {-1, -1, 0.0};

    #pragma omp parallel
    {
        FarthestInsertionTask localBestTask = {-1, -1, 0.0};

        #pragma omp for nowait
        for (int j = 0; j < numOfCoords; j++) {
            bool alreadyInSeq = false;
            for (int k = 0; k < seqValidLen; k++) {
                if (seq[k] == j) {
                    alreadyInSeq = true;
                    break;
                }
            }

            if (!alreadyInSeq) {
                double currentMaxDist = 0.0;
                int currentInsertIndex = -1;
                for (int i = 0; i < seqValidLen; i++) {
                    if (seq[i] != -1 && dist[seq[i]][j] > currentMaxDist) {
                        currentMaxDist = dist[seq[i]][j];
                        currentInsertIndex = i;
                    }
                }

                if (currentMaxDist > localBestTask.distance) {
                    localBestTask = (FarthestInsertionTask){j, currentInsertIndex, currentMaxDist};
                }
            }
        }

        #pragma omp critical
        {
            if (localBestTask.distance > globalBestTask.distance) {
                globalBestTask = localBestTask;
            }
        }
    }

    // 确定插入位置
    double minInsertionCost = DBL_MAX;
    int bestInsertIndex = globalBestTask.insertIndex;
    for (int i = 0; i < seqValidLen - 1; i++) {
        int nextIndex = (i + 1 < seqValidLen) ? seq[i + 1] : 0;
        double insertionCost = dist[seq[i]][globalBestTask.farthestPoint] + dist[globalBestTask.farthestPoint][nextIndex] - dist[seq[i]][nextIndex];
        if (insertionCost < minInsertionCost) {
            minInsertionCost = insertionCost;
            bestInsertIndex = i + 1;
        }
    }

    globalBestTask.insertIndex = bestInsertIndex;
    return globalBestTask;
}









void insertPoint(int *seq, int seqLen, FarthestInsertionTask task) {
    if (task.insertIndex < seqLen + 1) {
        for (int i = seqLen; i >= task.insertIndex; i--) {
            seq[i + 1] = seq[i];
        }
        seq[task.insertIndex] = task.farthestPoint;
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
    while (currentSeqLen < numOfCoords + 1) {
        FarthestInsertionTask task = findFarthestInsertion(resultSeq, dist, numOfCoords, currentSeqLen);
        printf("Task: farthestPoint = %d, insertIndex = %d, distance = %f\n", task.farthestPoint, task.insertIndex, task.distance);

        if (task.farthestPoint != -1) {
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
    resultSeq[currentSeqLen] = 0;
    currentSeqLen++;

    // 输出最终序列
    printf("\nFinal sequence: ");
    for (int i = 0; i < currentSeqLen; i++) {
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