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

int readNumOfCoords(char *inputFileName);
double **readCoords(char *filename, int numOfCoords);

/*Gets the number of the coordinates in the file. Returns as a single integer*/
int readNumOfCoords(char *filename)
{
	FILE *file = fopen(filename, "r");
	int numOfCoords = 0;

	if (file == NULL)
	{
		return -1;
	}

	char line[100];

	while (fgets(line, sizeof(line), file) != NULL)
	{
		numOfCoords++;
	}

	return numOfCoords;
}

/*Gets the data from the file. Returns as an array of doubles, Ignores the first integer*/
double **readCoords(char *filename, int numOfCoords)
{
	FILE *file = fopen(filename, "r");
	int i;

	char line[100];

	if (file == NULL)
	{
		printf("Unable to open file: %s", filename);
		return NULL;
	}

	double **coords = (double **)malloc(numOfCoords * sizeof(double *));

	for (i = 0; i < numOfCoords; i++)
	{
		coords[i] = (double *)malloc(2 * sizeof(int));
		if (coords[i] == NULL)
		{
			perror("Memory Allocation Failed");
		}
	}

	int lineNum = 0;
	while (fgets(line, sizeof(line), file) != NULL)
	{
		double x, y;
		if (sscanf(line, "%lf,%lf", &x, &y) == 2)
		{
			coords[lineNum][0] = x;
			coords[lineNum][1] = y;
			lineNum++;
		}
	}

	return coords;
}

void *writeTourToFile(int *tour, int tourLength, char *filename)
{

	FILE *file = fopen(filename, "w");
	int i;

	if (file == NULL)
	{
		printf("Unable to open file: %s", filename);
		return NULL;
	}

	fprintf(file, "%d \n", tourLength);

	printf("Writing output data\n");
	for (i = 0; i < tourLength; i++)
	{
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

double const DBL_MAX = 99999.99999;



InsertionTask findCheapestInsertion(int *seq, double **dist, int numOfCoords, int seqValidLen)
{
	InsertionTask globalBestTask = {-1, -1, DBL_MAX};

#pragma omp parallel
	{
		InsertionTask localBestTask = {-1, -1, DBL_MAX};

#pragma omp for nowait
		for (int i = 0; i < seqValidLen; i++)
		{
			for (int tempPoint = 0; tempPoint < numOfCoords; tempPoint++)
			{
				bool alreadyInSeq = false;
				for (int k = 0; k < seqValidLen; k++)
				{
					if (seq[k] == tempPoint)
					{
						alreadyInSeq = true;
						break;
					}
				}

				if (!alreadyInSeq)
				{
					int nextIndex = (i + 1 < seqValidLen) ? seq[i + 1] : 0;
					double currentCost = dist[seq[i]][tempPoint] + dist[tempPoint][nextIndex] - dist[seq[i]][nextIndex];

					if (currentCost < localBestTask.cost)
					{
						localBestTask = (InsertionTask){tempPoint, i + 1, currentCost};
					}
				}
			}
		}

#pragma omp critical
		{
			if (localBestTask.cost < globalBestTask.cost)
			{
				globalBestTask = localBestTask;
			}
		}
	}

	return globalBestTask;
}

void insertPoint(int *seq, int seqLen, InsertionTask task)
{
	if (task.insertIndex < seqLen + 1)
	{
#pragma omp parallel for
		for (int i = seqLen; i >= task.insertIndex; i--)
		{
			seq[i + 1] = seq[i];
		}
		seq[task.insertIndex] = task.insertPoint;
	}
}

// void print2DArray(double **arr, int rows, int columns){
// 	// int rowLength = sizeof(arr) / sizeof(arr[0]);
// 	// int colLength = sizeof(arr[0]) / sizeof(arr[0][0]);
// 	printf("\n---Array Value---\n");
// 	int i = 0;
// 	int j = 0;
// 	for (i = 0; i < rows; i++)
// 	{
// 		for (j = 0; j < columns; j++)
// 		{
// 			printf("%011.5f, ", arr[i][j]);
// 		}
// 		printf("\n");
// 	}
// }

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

int main(int argc, char *argv[])
{
	clock_t startTime = clock();

	// 初始化和读取输入数据
	char *inputFileName = "16_coords.coord"; // 根据实际情况调整文件名
	int numOfCoords = readNumOfCoords(inputFileName);
	double **inputs = readCoords(inputFileName, numOfCoords);

	// 初始化距离矩阵
	double **dist = (double **)malloc(numOfCoords * sizeof(double *));
#pragma omp parallel for
	for (int i = 0; i < numOfCoords; i++)
	{
		dist[i] = (double *)malloc(numOfCoords * sizeof(double));
	}
	calculateDist(inputs, dist, numOfCoords);

	// 初始化序列，假设起始点为0
	int *resultSeq = (int *)malloc((numOfCoords + 2) * sizeof(int)); // +2 for start and end
	resultSeq[0] = 0;												 // 起始点
#pragma omp parallel for
	for (int i = 1; i <= numOfCoords; i++)
	{
		resultSeq[i] = -1; // 初始化为-1
	}
	resultSeq[numOfCoords + 1] = 0; // 结束点，也为0
	int currentSeqLen = 1;			// 当前序列长度，初始包含起始点

	for (int i = 0; i < numOfCoords; i++)
	{
		InsertionTask task = findCheapestInsertion(resultSeq, dist, numOfCoords, currentSeqLen);
		if (task.insertPoint != -1)
		{
			insertPoint(resultSeq, currentSeqLen, task);
			currentSeqLen++;
		}

		if (currentSeqLen % 100 == 0){
			// 打印当前序列
			// printf("Current sequence length: %d\n", currentSeqLen);
			// for (int j = 0; j < currentSeqLen; j++)
			// {
			// 	printf("%d ", resultSeq[j]);
			// }
			// printf("\n");
			printf("\nTIME: length: %d, time: %f", currentSeqLen, ((double)(clock() - startTime)) / 1000);
		}
		
	}

	// 添加结束点
	resultSeq[currentSeqLen] = 0;
	currentSeqLen++;

	// 输出最终序列
	printf("\nFinal sequence: ");
	for (int i = 0; i < currentSeqLen; i++)
	{
		printf("%d ", resultSeq[i]);
	}

	// 清理资源
#pragma omp parallel for
	for (int i = 0; i < numOfCoords; i++)
	{
		free(dist[i]);
	}
	free(dist);
	free(inputs);
	free(resultSeq);

	printf("\nTIME: %f", ((double)(clock() - startTime)));

	return 0;
}