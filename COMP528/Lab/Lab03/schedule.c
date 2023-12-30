#include<stdio.h>
#include<omp.h>

int main(void) {

	int threadID;

	int dimSize = 5;

	int lMatrix[dimSize][dimSize];

	int vec[dimSize];

	int res[dimSize][dimSize];

	int i;
	int j;


	for (i = 0; i < dimSize; i++) {
		for (j = 0; j <= i; j++) {
			lMatrix[i][j] = rand() % 10;
		}
		vec[i] = rand() % 10;
	}

#pragma omp parallel shared(vec, lMatrix, dimSize, res) private(i, j, threadID)
	{

		threadID = omp_get_thread_num();

		if (threadID == 0) {
			printf("Static default\n");
		}

#pragma omp for
		
			for (i = 0; i < dimSize; i++)
			{
				for (j = 0; j <= i; j++)
				{
					res[i][j] += lMatrix[i][j] * vec[j];
					printf("Thread %d: i: %d, j: %d, res[i][j]: %d\n", threadID, i, j, res[i][j]);
				}
			}
		


	}

#pragma omp parallel shared(vec, lMatrix, dimSize, res) private(i, j, threadID)
	{
		threadID = omp_get_thread_num();


		if (threadID == 0) {
			printf("Static 2 chunks\n");
		}

#pragma omp for
		
			for (i = 0; i < dimSize; i++)
			{
				for (j = 0; j <= i; j++)
				{
					res[i][j] += lMatrix[i][j] * vec[j];
					printf("Thread %d: i: %d, j: %d, res[i][j]: %d\n", threadID, i, j, res[i][j]);
				}
			}
		

	}


#pragma omp parallel shared(vec, lMatrix, dimSize, res) private(i, j, threadID)
	{
		threadID = omp_get_thread_num();
		if (threadID == 0) {
			printf("Static 4 chunks\n");
		}

#pragma omp for
			for (i = 0; i < dimSize; i++)
			{
				for (j = 0; j <= i; j++)
				{
					res[i][j] += lMatrix[i][j] * vec[j];
					printf("Thread %d: i: %d, j: %d, res[i][j]: %d\n", threadID, i, j, res[i][j]);
				}
			}
	}

}
