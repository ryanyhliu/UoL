#include <stdio.h>
#include <mpi.h>
int main(void) {
	int myRank;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	printf("Hi from rank %d\n", myRank);
	MPI_Finalize();
}
