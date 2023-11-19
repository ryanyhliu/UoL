#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
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

void *writeTourToFile(int *path, int tourLength, char *filename)
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
		fprintf(file, "%d ", path[i]);
	}
}

////////////////// YH's Code Begin

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
	// double temp = sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
	// printf("\n___DEBUG___: temp = %f\n", temp);
	return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

void calculateDist(double **inputs, double **dist, int rows, int columns)
{
	int i = 0;
	int j = 0;

	// 计算距离
	for (i = 0; i < rows; i++)
	{
		dist[i][i] = 0.0;
		for (j = i + 1; j < rows; j++)
		{
			dist[i][j] = euclideanDistance(inputs[i], inputs[j]);
			dist[j][i] = dist[i][j];
		}
	}
}



int *getCheapestPath(double **dist, int numOfCoords)
{
	int *path = (int *)malloc((numOfCoords + 1) * sizeof(int));
	path[0] = 0;
	int pathLen = 1;

	// 是否已经使用过该点, 0:no 1:yes
	int *used = (int *)calloc(numOfCoords, sizeof(int));
	used[0] = 1;


	while (pathLen < numOfCoords)
	{
		int allMinIndex = -1; // 全局最小值的点
		double allMin = 99999.99999; // 全局最小值
		int allInsertIndex = -1; // 全局最小值插入的地方

#pragma omp parallel
		{
			int currentMinIndex = -1; // 当前最小值的点
			double currentMin = 99999.99999; // 当前找到的最小的值
			int currentInsertIndex = -1; // 当前点插入点的地方


#pragma omp for nowait
			for (int i = 0; i < numOfCoords; i++)
			{
				if (used[i] != 1) // 没用过的点
				{
					for (int j = 0; j < pathLen; j++)
					{
						int tempPoint = (j + 1) % pathLen;

						double currentDist = dist[path[j]][i] + dist[i][path[tempPoint]] - dist[path[j]][path[tempPoint]];
							if (currentDist < currentMin)
							{
								currentMinIndex = i;
								currentMin = currentDist;
								currentInsertIndex = j + 1;
							}
						
						
					}
				}
			}

#pragma omp critical
			{
				if (allMin > currentMin)
				{
					allMinIndex = currentMinIndex;
					allMin = currentMin;
					allInsertIndex = currentInsertIndex;
				}
			}
		}

		// Insertion
		for (int i = pathLen; i >= allInsertIndex; i--)
		{
			path[i] = path[i - 1];
		}
		path[allInsertIndex] = allMinIndex;
		used[allMinIndex] = 1;
		pathLen++;
	}

	// 插入最后一个点
	path[numOfCoords] = path[0];

	free(used);
	return path;
}

int main(int argc, char *argv[])
{
	printf("YHCode Start\n");
	double startTime = clock();

	int i = 0;
	int j = 0;

	// 读取文件中数组
	// char *inputFileName = argv[0];
	// char *outputFileName = argv[1];
	char *inputFileName = "16_coords.coord";
	char *outputFileName = "cout";
	int numOfCoords = readNumOfCoords(inputFileName);

	double **inputs = readCoords(inputFileName, numOfCoords); // 得到二维数组
	// print2DArray(inputs, numOfCoords, 2);

	// 距离矩阵
	// 初始化行长度, 不然直接赋值dist[i][j]会出错
	double **dist = (double **)malloc(numOfCoords * sizeof(double *));
	// 初始化列长度
	for (i = 0; i < numOfCoords; i++)
	{
		dist[i] = (double *)malloc(numOfCoords * sizeof(double));
	}

	calculateDist(inputs, dist, numOfCoords, numOfCoords);
	// print2DArray(dist, numOfCoords, numOfCoords);

	// 最终序列 (路径)
	int *resultSeq = (int *)malloc((numOfCoords + 1) * sizeof(int)); // 路径 0 -> 1 -> 2 -> 0, 长度比坐标+1


	// // 初始化 -1
	// for (i = 0; i < numOfCoords + 1; i++)
	// {
	// 	resultSeq[i] = -1;
	// }
	// // 0 -> 0
	// resultSeq[0] = 0;
	// resultSeq[1] = 0;
	// resultSeq = getFarthestPoint(resultSeq, dist, numOfCoords);



	resultSeq = getCheapestPath(dist, numOfCoords);

	printf("\nResult: ");
	for (i = 0; i < numOfCoords + 1; i++)
	{
		printf("%d ", resultSeq[i]);
	}

	writeTourToFile(resultSeq, numOfCoords + 1, outputFileName);


    printf("Total TIME: %f \n", clock() - startTime);

	return 0;
}
