#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>

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

double euclideanDistance(double *a, double *b){
	// double temp = sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
	// printf("\n___DEBUG___: temp = %f\n", temp);
	return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

void calculateDist(double **inputs, double** dist, int rows, int columns){
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

/**
 * @brief 最远插入法 fInsertion
 * @param seq 当前序列, 寻找下一个点之前, 要排除已有的坐标
 * @param dist
 * @param numOfCoords 总坐标个数
 * @return 下一个坐标 (要加入到序列里)
*/
int getFarthestPoint(int *seq,  double **dist, int numOfCoords){
	int i = 0;
	int j = 0;
	// 输出序列总长度 (总步数)
	int seqTotalLen = numOfCoords + 1;
	// 当前序列长度 (当前步数)
	int seqValidLen = 0;

#pragma omp parallel

	// 实际长度 (!= -1 元素个数)
	for (i = 0; i < seqTotalLen; i++)
	{
		if (seq[i] != -1)
		{
			seqValidLen += 1;
		}
	}
	
	// Implement a Set -> Exclude used coordinate
	// Initialize Set
	int *setSeq = (int *)malloc(seqTotalLen * sizeof(int));
	for (i = 0; i < seqTotalLen; i++){
		setSeq[i] = -1; // Unused
	}
	// Insert Set from seq
	for (i = 0; i < seqTotalLen; i++){
		if (seq[i] != -1)
		{
			setSeq[seq[i]] = 0; // Used
		}
	}


	// 遍历图上 -> 不在图上, 最远距离的点
	double tempFarthest = 0.0;
	int tempFarthestPoint = -1;
#pragma omp for nowait
	for (i = 0; i < seqValidLen; i++){ // 图上的点
		for (j = 0; j < numOfCoords; j++){
			if (setSeq[j] == -1){ // 不在图上的点
				if (tempFarthest < dist[seq[i]][j]){
					tempFarthest = dist[seq[i]][j];
					tempFarthestPoint = j;
					// printf("\n---DEBUG---: Farthest dist %d -> (%d) = %011.5f",seq[i], j, tempFarthest);
				}
			}
		}
	}


	// 要插入的地方
	double tempClosest = 99999.99999;
	int tempInsertIndex = -1;
	// 把这个点加入到图中的某条边 欧氏距离最短的那条
#pragma omp for nowait
	for (i = 0; i < seqValidLen - 1; i++){
		j = i + 1;
		// printf("\n---DEBUG---: Current judging: %d -> %d", seq[i], seq[j]);
		// printf("\n---DEBUG---: (%d -> %d = %011.5f)", seq[i], seq[j], dist[seq[i]][tempFarthestPoint] + dist[tempFarthestPoint][seq[j]] - dist[seq[i]][seq[j]]);

		if (tempClosest > (dist[seq[i]][tempFarthestPoint] + dist[tempFarthestPoint][seq[j]] - dist[seq[i]][seq[j]])){
			tempClosest = (dist[seq[i]][tempFarthestPoint] + dist[tempFarthestPoint][seq[j]] - dist[seq[i]][seq[j]]);
			tempInsertIndex = i + 1;
		}
	}
	// printf("\n---DEBUG---: Insert position = %d, point = %d", tempInsertIndex, tempFarthestPoint);


	// Insertion
#pragma omp critical
	for (i = seqValidLen - 1; i >= tempInsertIndex; i--){
		seq[i + 1] = seq[i];
	}
	seq[tempInsertIndex] = tempFarthestPoint;

	return seqValidLen + 1;
}



int *findShortestTour(double **distanceMatrix, int numOfCoords)
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

	
int main(void){
	printf("YHCode\n");
	int i = 0;
	int j = 0;
	
	// 读取文件中数组
	char fileName[] = "16_coords.coord";
	int numOfCoords = readNumOfCoords(fileName);
	double **inputs = readCoords(fileName, numOfCoords); // 得到二维数组
	// print2DArray(inputs, numOfCoords, 2);

	// 距离矩阵
	// 初始化行长度, 不然直接赋值dist[i][j]会出错
	double **dist = (double **)malloc(numOfCoords * sizeof(double *));
	// 初始化列长度
	for (i = 0; i < numOfCoords; i++) {
        dist[i] = (double *)malloc(numOfCoords * sizeof(double));
    }
	
	calculateDist(inputs, dist, numOfCoords, numOfCoords);
	// print2DArray(dist, numOfCoords, numOfCoords);
	

	// 最终序列 (路径)
	int *resultSeq = (int *)malloc((numOfCoords + 1) * sizeof(int)); // 路径 0 -> 1 -> 2 -> 0, 长度比坐标+1
	// // 初始化 -1
	// for (i = 0; i < numOfCoords + 1; i++){
	// 	resultSeq[i] = -1;
	// }

	// // 0 -> 0
	// resultSeq[0] = 0;
	// resultSeq[1] = 0; 


	// int validLenOfSeq = 2;
	// while (validLenOfSeq < numOfCoords + 1)
	// {
	// 	validLenOfSeq = getFarthestPoint(resultSeq, dist, numOfCoords);
	// 	// printf("\n---DEBUG---: validLenOfSeq = %d", validLenOfSeq);
	// 	printf("\nResult: ");
	// 	for (i = 0; i < numOfCoords + 1; i++){
	// 		printf("%d ", resultSeq[i]);
	// 	}
	// }
	resultSeq = findShortestTour(dist, numOfCoords);
	
	printf("\nResult: ");
	for (i = 0; i < numOfCoords + 1; i++){
		printf("%d ", resultSeq[i]);
	}
	

	// char outputFileName[] = "output.txt";
	// writeTourToFile(tour, tourLength, outputFileName);

	return 0;
}

