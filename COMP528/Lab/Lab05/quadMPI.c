#include<stdio.h>
#include<mpi.h>

double func(double);
double independentRankWork(int commSize, int myRank);
void integrationPTP(int commSize, int myRank);

int main(void) {
	const int REPEAT = 5;

	double tStart;
	double tEnd;

	/*Set up MPI Environment here*/
	int commSize = 0;
	int myRank = 0;


	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	int count = 0;
	for (count; count < REPEAT; count++) {

		

		tStart = MPI_Wtime();

		integrationPTP(commSize, myRank);

		tEnd = MPI_Wtime();


		printf("COUNT = %d\n", count);
		printf("Time taken = %f milliseconds\n", 1000.0 * (tEnd - tStart));	//print wallTimeTaken

		

	}

	/*Finalise MPI Environment here*/
	MPI_Finalize();
}

void integrationPTP(int commSize, int myRank) {
	/*
	Type your code here to find the size of the area
	underneath the code using Point-To-Point communications
	Each process should call independentRankWork and send their result
	to process 0, which will add the results as they are received
	*/

	double rank0_sumValue = 0.0;

	// Calculate the num
	double num = independentRankWork(commSize, myRank);


	// According to Rand, decide Send / Receive, then add to sum
	if (myRank == 0) {
		rank0_sumValue += num;

		int i = 1;
		for (i = 1; i < commSize; i++) {
			double receiveValue = 0.0;

			MPI_Recv(&receiveValue, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			rank0_sumValue += receiveValue;
		}
	}
	else {

		// Send to rank0
		MPI_Send(&num, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	// if rank0, print
	if (myRank == 0) {
		printf("SumValue = %f\n", rank0_sumValue);
	}
}

double independentRankWork(int commSize, int myRank) {
	/*Set up the number of quads*/
	const double a = 0.0, b = 200.0;
	const int  quads = 50000;
	double const width = (b - a) / (double)quads;

	int quadsPerRank = quads / commSize;

	if (myRank == commSize - 1) {	//final rank leaving this here for ease
		quadsPerRank = quads - (quadsPerRank * myRank);
	}

	int startIter = myRank * quadsPerRank;
	int endIter = startIter + quadsPerRank - 1;

	double x, y;

	double sum;
	int i;

	for (i = startIter; i <= endIter; i++) {
		x = a + i * width;
		y = x + width;
		sum += 0.5 * width * (func(x) + func(y));
	}
	return sum;
}

double func(double x){
	return x * x;
}

