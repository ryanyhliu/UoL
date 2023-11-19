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

double const DBL_MIN = 0.0;
double const DBL_MAX = 99999.99999;

InsertionTask findFarthestInsertion(int *seq, double **dist, int numOfCoords, int seqValidLen)
{
	InsertionTask globalBestTask = {-1, -1, -1.0};

	// 初始化已在序列中的点的集合
	bool *inSeq = (bool *)malloc(numOfCoords * sizeof(bool));
#pragma omp parallel for
	for (int i = 0; i < numOfCoords; i++)
	{
		inSeq[i] = false;
	}
#pragma omp parallel for
	for (int i = 0; i < seqValidLen; i++)
	{
		inSeq[seq[i]] = true;
	}

	// 寻找最远点

	InsertionTask localBestTask = {-1, -1, -1.0};

#pragma omp parallel for
	for (int i = 0; i < seqValidLen; i++)
	{
#pragma omp parallel for
		for (int j = 0; j < numOfCoords; j++)
		{
			if (!inSeq[j])
			{
				double currentDist = dist[seq[i]][j];
				if (currentDist > localBestTask.cost)
				{
					localBestTask.insertPoint = j;
					localBestTask.cost = currentDist;
					localBestTask.insertIndex = i + 1; // 这里预设一个插入位置
				}
			}
		}
	}

#pragma omp critical
	{
		if (localBestTask.cost > globalBestTask.cost)
		{
			globalBestTask = localBestTask;
		}
	}

	free(inSeq); // 释放集合内存

	// 并行化重新计算最佳插入位置
	InsertionTask bestTaskInThread;
#pragma omp parallel private(bestTaskInThread)
	{
		bestTaskInThread.insertIndex = -1;
		bestTaskInThread.insertPoint = globalBestTask.insertPoint;
		bestTaskInThread.cost = DBL_MAX;

#pragma omp for nowait
		for (int i = 0; i < seqValidLen - 1; i++)
		{
			int j = i + 1;
			double insertCost = dist[seq[i]][globalBestTask.insertPoint] + dist[globalBestTask.insertPoint][seq[j]] - dist[seq[i]][seq[j]];
			if (insertCost < bestTaskInThread.cost)
			{
				bestTaskInThread.cost = insertCost;
				bestTaskInThread.insertIndex = i + 1;
			}
		}

#pragma omp critical
		{
			if (bestTaskInThread.cost < globalBestTask.cost)
			{
				globalBestTask = bestTaskInThread;
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

int *getCheapestPath(double **distanceMatrix, int numOfCoords)
{
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));
    int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); // +1 for the starting coordinate at the end
    int tourSize = 1;                                           // Start with the first coordinate
    visited[0] = true;                                          // Start coordinate is visited
    tour[0] = 0;                                                // Start at the first coordinate

    while (tourSize < numOfCoords)
    {
        double globalMinCost = INFINITY;
        int globalMinIndex = -1;
        int globalInsertPosition = -1;

#pragma omp parallel
        {
            double localMinCost = INFINITY;
            int localMinIndex = -1;
            int localInsertPosition = -1;

// Parallel loop, each thread calculates its own minimum cost and related index
#pragma omp for nowait
            for (int i = 0; i < numOfCoords; i++)
            {
                if (!visited[i])
                {
                    for (int j = 0; j < tourSize; j++)
                    {
                        int k = (j + 1) % tourSize;
                        double cost = distanceMatrix[tour[j]][i] + distanceMatrix[i][tour[k]] - distanceMatrix[tour[j]][tour[k]];
                        if (cost < localMinCost)
                        {
                            localMinCost = cost;
                            localMinIndex = i;
                            localInsertPosition = j + 1;
                        }
                    }
                }
            }

// Single thread to update minimum values
#pragma omp critical
            {
                if (localMinCost < globalMinCost)
                {
                    globalMinCost = localMinCost;
                    globalMinIndex = localMinIndex;
                    globalInsertPosition = localInsertPosition;
                }
            }
        }

        // Use global minimum value to update tour
        for (int i = tourSize; i >= globalInsertPosition; i--)
        {
            tour[i] = tour[i - 1];
        }
        tour[globalInsertPosition] = globalMinIndex;
        visited[globalMinIndex] = true;
        tourSize++;
    }

    tour[numOfCoords] = tour[0];

    free(visited);
    return tour;
}

int main(int argc, char *argv[])
{
	clock_t startTime = clock();

	// 初始化和读取输入数据
	// char *inputFileName = argv[0];
	// char *outputFileName = argv[1];
	char *inputFileName = "16_coords.coord"; // 根据实际情况调整文件名
	char *outputFileName = "out.txt";
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

	// 初始化序列，包括起始点
	int *resultSeq = (int *)malloc((numOfCoords + 1) * sizeof(int));
// 	resultSeq[0] = 0;
// #pragma omp parallel for
// 	for (int i = 1; i <= numOfCoords; i++)
// 	{
// 		resultSeq[i] = -1;
// 	}

// 	int currentSeqLen = 1;

// 	for (; currentSeqLen < numOfCoords;)
// 	{
// 		InsertionTask task = findFarthestInsertion(resultSeq, dist, numOfCoords, currentSeqLen);

// 		if (task.insertPoint != -1)
// 		{
// 			insertPoint(resultSeq, currentSeqLen, task);
// 			currentSeqLen++;
// 		}

// 		// 打印当前序列
// 		if (currentSeqLen % 10 == 0)
// 		{
// 			printf("\nTIME: length: %d, time: %f", currentSeqLen, ((double)(clock() - startTime)) / 1000);
// 		}
// 	}

// 	// 添加结束点
// 	resultSeq[numOfCoords] = 0;

	resultSeq = getCheapestPath(dist, numOfCoords);

	// 输出最终序列
	printf("\nFinal sequence: ");
	for (int i = 0; i < numOfCoords + 1; i++)
	{
		printf("%d ", resultSeq[i]);
	}

	// 写入文件
	writeTourToFile(resultSeq, numOfCoords + 1, outputFileName);

	// 清理资源
#pragma omp parallel for
	for (int i = 0; i < numOfCoords; i++)
	{
		free(dist[i]);
	}
	free(dist);
	free(inputs);
	free(resultSeq);

	printf("\nTIME: %f", ((double)(clock() - startTime) / CLOCKS_PER_SEC));
	return 0;
}