#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>
#include<omp.h>
#include<time.h>

/*This code is for reading and writing to files for the 2023-24 COMP528 CA1*/

/*Use the functions in this file to read from the input file, and write to the output file*/

/*You should use this file when compiling your code*/

/*Declare these functions at the top of each 'main' file*/

/*If there are any issues with this code, please contact: h.j.forbes@liverpool.ac.uk*/

int readNumOfCoords(char *inputFileName);
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
	return sqrt(pow(a[0] - b[0], 2) + pow(a[1] - b[1], 2));
}

void calculateDist(double **inputs, double** dist, int rows, int columns){
	int i = 0;
	int j = 0;

	// 计算距离
	#pragma omp parallel for // 并行化外层循环
	for (i = 0; i < rows; i++)
	{
		dist[i][i] = 0.0;
		#pragma omp parallel for // 并行化内层循环
		for (j = i + 1; j < rows; j++)
		{
			// #pragma omp critical
			{
				dist[i][j] = euclideanDistance(inputs[i], inputs[j]);
				dist[j][i] = dist[i][j];
			}
		}
	}
}

/**
 * @brief 最便宜插入法 cInsertion
 * @param seq 当前序列, 寻找下一个点之前, 要排除已有的坐标
 * @param dist
 * @param numOfCoords 总坐标个数
 * @return 当前长度
*/
int getCheapestPoint(int *seq,  double **dist, int numOfCoords){
	// printf("\n---TEST---: Begin cInsertion");

	int i = 0;
	int j = 0;
	// 输出序列总长度 (总步数)
	int seqTotalLen = numOfCoords + 1;
	// 当前序列长度 (当前步数)
	int seqValidLen = 0;


	// 实际长度 (!= -1 元素个数)
	#pragma omp parallel for // 并行化外层循环
	for (i = 0; i < seqTotalLen; i++)
	{
		// #pragma omp critical(c01)
		{
			if (seq[i] != -1)
			{
				seqValidLen += 1;
			}
		}
	}
	
	// Implement a Set -> Exclude used coordinate
	// Initialize Set
	int *setSeq = (int *)malloc(seqTotalLen * sizeof(int));
	#pragma omp parallel for // 并行化外层循环
	for (i = 0; i < seqTotalLen; i++){
		// #pragma omp critical(c02)
		{
			setSeq[i] = -1; // Unused
		}
	}
	// Insert Set from seq
	#pragma omp parallel for // 并行化外层循环
	for (i = 0; i < seqTotalLen; i++){
		if (seq[i] != -1)
		{
			// #pragma omp critical(c03)
			{
				setSeq[seq[i]] = 0; // Used
			}
		}
	}

	// printf("\n---TEST---: Init Set over");


	double tempAllPathCheapest = 99999.99999;
	double tempCurrentPathCheapest = 99999.99999;
	int tempAllPathCheapestPoint = -1;
	int tempCurrentPathCheapestPoint = -1;
	int tempInsertIndex = -1;
	int tempPoint = -1;

	int max_i = seqValidLen - 1; // 优化
	#pragma omp parallel for // 并行化外层循环
	for (i = 0; i < max_i; i++){ // 一共要判断 路径 = 当前节点数(有效长度) - 1
		// #pragma omp critical(c04)
		{
			j = i + 1;
			tempCurrentPathCheapest = 99999.99999;
			#pragma omp parallel for // 并行化内层循环
			for (tempPoint = 0; tempPoint < numOfCoords; tempPoint++){
				// #pragma omp critical(c05)
				{
					if (setSeq[tempPoint] == -1){ // Unused
						// printf("\n---DEBUG---: (%d -> %d -> %d) = %011.5f", seq[i], tempPoint, seq[j], dist[seq[i]][tempPoint] + dist[tempPoint][seq[j]] - dist[i][j]);
						
						if (tempCurrentPathCheapest > dist[seq[i]][tempPoint] + dist[tempPoint][seq[j]] - dist[seq[i]][seq[j]]){
							tempCurrentPathCheapest = (dist[seq[i]][tempPoint] + dist[tempPoint][seq[j]] - dist[seq[i]][seq[j]]);
							tempCurrentPathCheapestPoint = tempPoint;
						}
					}
				}
			}
			// printf("\n---DEBUG---: currentPathCheapest = %011.5f", tempCurrentPathCheapest);
			// printf("\n---DEBUG---: allPathCheapest = %011.5f", tempAllPathCheapest);

			// Record current path: Insert index & value
			if (tempAllPathCheapest > tempCurrentPathCheapest){
				tempAllPathCheapest = tempCurrentPathCheapest;
				tempAllPathCheapestPoint = tempCurrentPathCheapestPoint;
				tempInsertIndex = i + 1; // 把 Point 插入到 tempInsertIndex + 1 的位置, 后边的往后挪
				
			}
		}
	}

	// // Insertion
	// #pragma omp parallel for // 并行化外层循环
	// for (i = seqValidLen - 1; i >= tempInsertIndex; i--){
	// 	#pragma omp critical(c06)
	// 	{
	// 		seq[i + 1] = seq[i];
	// 	}
	// }
	// seq[tempInsertIndex] = tempAllPathCheapestPoint;
	// printf("\n---DEBUG---: Seq[%d] = %d", tempInsertIndex, tempAllPathCheapestPoint);

	// 使用额外的数组进行并行处理
	int *newSeq = (int *)malloc((seqTotalLen + 1) * sizeof(int));
	memcpy(newSeq, seq, seqTotalLen * sizeof(int)); // 复制原始数组

	#pragma omp parallel for
	for (i = seqValidLen - 1; i >= tempInsertIndex; i--) {
		newSeq[i + 1] = seq[i];
	}
	newSeq[tempInsertIndex] = tempAllPathCheapestPoint;

	// 将新数组复制回原数组
	memcpy(seq, newSeq, (seqTotalLen + 1) * sizeof(int));
	free(newSeq);

	
	// printf("\n---TEST---: Add point to queue over");


	free(setSeq);
	return seqValidLen + 1;
}


	
int main(int argc, char *argv[]){
	clock_t startTime = clock(); // 计算运行时间
	int i = 0; // 循环用
	int j = 0; // 循环用
	
	char *inputFileName = "16_coords.coord"; // 输入文件名
	char *outputFileName = "myout.dat";
	// char *inputFileName  = argv[1];  // 输入文件名
	// char *outputFileName = argv[2];  // 输出文件名
	int numOfCoords = readNumOfCoords(inputFileName); // 坐标个数
	double **inputs = readCoords(inputFileName, numOfCoords); // 坐标
	// print2DArray(inputs, numOfCoords, 2);

	// printf("\n---TEST---: Read file over");


	
	// 初始化行长度
	double **dist = (double **)malloc(numOfCoords * sizeof(double *)); // 距离矩阵
	// 初始化列长度
	#pragma omp parallel for // 并行化外层循环
	for (i = 0; i < numOfCoords; i++) {
		// #pragma omp critical(c07)
		{
        	dist[i] = (double *)malloc(numOfCoords * sizeof(double));
		}
    }
	
	calculateDist(inputs, dist, numOfCoords, numOfCoords);
	free(inputs);
	// print2DArray(dist, numOfCoords, numOfCoords);

	// printf("\n---TEST---: Calculate dist array over");

	
	// 路径 0 -> 1 -> 2 -> 0, 长度比坐标+1
	int *resultSeq = (int *)malloc((numOfCoords + 1) * sizeof(int)); // 最终序列 (路径)
	// 初始化 -1
	int max_i = numOfCoords + 1;
	#pragma omp parallel for // 并行化外层循环
	for (i = 0; i < max_i; i++){
		// #pragma omp critical(c08)
		{
			resultSeq[i] = -1;
		}
	}

	// 插入开头和结尾 0 -> 0
	resultSeq[0] = 0;
	resultSeq[1] = 0; 
	int currentSteps = 2; // 当前步数

	int totalSteps = numOfCoords + 1; // 总步数 (坐标数 + 1)
	#pragma omp parallel shared(validLenOfSeq)
	{
		#pragma omp critical(c09)
		{
			while (currentSteps < totalSteps)
			{
				currentSteps = getCheapestPoint(resultSeq, dist, numOfCoords);
				printf("\n---DEBUG---: currentSteps = %d", currentSteps);
			}
		}
	}

	#pragma omp single
	{
		printf("\nResult: ");
		for (i = 0; i < numOfCoords + 1; i++){
			printf("%d ", resultSeq[i]);
		}
	}




	writeTourToFile(resultSeq, totalSteps, outputFileName);

	printf("\nTOTAL Seconds: %f ", ((double)clock() - startTime));
	return 0;
}

