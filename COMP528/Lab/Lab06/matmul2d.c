#include <mpi.h>
#include <stdio.h>

#define MAXSIZE 4 /* change the size of matrices as required*/

int X[MAXSIZE][MAXSIZE], Y[MAXSIZE][MAXSIZE], Z[MAXSIZE][MAXSIZE];

void fill_matrix(int mat[MAXSIZE][MAXSIZE])
{
    srand(time(NULL));
    /*TASK: Write code to fill the values in mat by making it equal to rand()%10*/
    int i = 0;
    int j = 0;
    for (i = 0; i < MAXSIZE; i++){
        for (j = 0; j < MAXSIZE; j++){
            mat[i][j] = rand() % 10; // [0, 9]
        }
    }
}

void print_matrix(int mat[MAXSIZE][MAXSIZE])
{
    int i, j;
    for (i = 0; i < MAXSIZE; i++)
    {
        for (j = 0; j < MAXSIZE; j++)
        {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int my_rank, comm_size, from, to;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);


    int part_size = MAXSIZE * MAXSIZE / comm_size;
    int localX[part_size][MAXSIZE], localY[part_size][MAXSIZE], localZ[part_size][MAXSIZE];
    int i, j, k;
    int root = 0;


    if (MAXSIZE % comm_size != 0)
    {
        exit(-1);
    }

    /*TASK: If the process is the root rank, fill both X and Y matrices*/
    if (my_rank == root){
        fill_matrix(X);
        sleep(1);
        fill_matrix(Y);
        
        print_matrix(X);
        print_matrix(Y);
    }

    from = my_rank * part_size;
    to = (my_rank + 1) * part_size;

    /*Root process sends the whole of Y to every process.*/
    MPI_Bcast(Y, MAXSIZE * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);

    /*Root process distributes parts of X to different processes*/
    MPI_Scatter(X, part_size, MPI_INT, localX, part_size, MPI_INT, root, MPI_COMM_WORLD);

    for (i = from; i < to; i++)
    {
        for (j = 0; j < MAXSIZE; j++)
        {
            localZ[i - from][j] = 0;
            for (k = 0; k < MAXSIZE; k++)
            {
                localZ[i - from][j] += localX[i - from][k] * Y[k][j];
            }
        }
    }

    /*TASK: Use MPI_Gather to pull all localZ from each process and store it in Z at the root process*/
    if (my_rank == root)
    {
        MPI_Gather(localZ, MAXSIZE * MAXSIZE / comm_size, MPI_INT, Z, MAXSIZE * MAXSIZE / comm_size, MPI_INT, root, MPI_COMM_WORLD);
    }else{
        MPI_Gather(localZ, MAXSIZE * MAXSIZE / comm_size, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
    }
    


    /*Task: If root print mat Z*/
    if (my_rank == root)
    {
        print_matrix(Z);
    }
    

    MPI_Finalize();
    return 0;
}