#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

#define MAXSIZE 1024 // 1024 /* change the size of matrices as required*/

int X[MAXSIZE][MAXSIZE], Y[MAXSIZE][MAXSIZE], Z[MAXSIZE][MAXSIZE];

void fill_matrix(int mat[MAXSIZE][MAXSIZE])
{
#pragma omp parallel
    {
        // srand(time(NULL));
        unsigned int seed = time(NULL) ^ omp_get_thread_num();
#pragma omp for 
        for (int i = 0; i < MAXSIZE; i++)
        {
            for (int j = 0; j < MAXSIZE; j++)
            {
                // mat[i][j] = rand() % 10;
                mat[i][j] = rand_r(&seed) % 10;
            }
        }
    }
}

void print_matrix(int mat[MAXSIZE][MAXSIZE])
{
    for (int i = 0; i < MAXSIZE; i++)
    {
        for (int j = 0; j < MAXSIZE; j++)
        {
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }
}

void convert2Dto1D(int **arr2D, int *arr1D){
    // int *arr1D = (int *)malloc(MAXSIZE * MAXSIZE * sizeof(int));
    int index = 0;
    int i;
    int j;
    for (i = 0; i < MAXSIZE; i++){
        for (j = 0; j < MAXSIZE; j++){
            arr1D[index++] = arr2D[i][j];
        }
    }
}

void **convert1Dto2D(int *arr1D, int **arr2D){
    // int arr2D[MAXSIZE][MAXSIZE];
    int index = 0;
    int i;
    int j;
    
    for (i = 0; i < MAXSIZE; i++){
        for (j = 0; j < MAXSIZE; j++){
            arr2D = arr1D[index++];
        }
    }
}


int main(int argc, char *argv[])
{
    printf("Before MPI init\n");
    MPI_Init(&argc, &argv);
    printf("After MPI init\n");
    double tStart, tEnd;

    tStart = MPI_Wtime();

    int my_rank, comm_size, from, to;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int part_size = MAXSIZE * MAXSIZE / comm_size;
    int localX[part_size][MAXSIZE], localY[part_size][MAXSIZE], localZ[part_size][MAXSIZE];
    int localX_1D[part_size], localY_1D[part_size], localZ_1D[part_size];
    int i, j, k;
    int root = 0;

    if (MAXSIZE % comm_size != 0)
    {
        exit(-1);
    }

    // from = my_rank * MAXSIZE / comm_size;
    // to = (my_rank + comm_size) * MAXSIZE / comm_size;
    from = my_rank * part_size;
    to = (my_rank + 1) * part_size;

    /*if root rank, fill the matrices X and Y*/

    if (my_rank == root)
    {
        fill_matrix(X);
        sleep(1);
        fill_matrix(Y);
        print_matrix(X);
        printf("\n");
        print_matrix(Y);
        printf("\n");
    }

    /*What's the difference here between MPI_Bcast and MPI_Scatter*/

    // MPI_Scatter(X, part_size * MAXSIZE, MPI_INT, localX, part_size * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);
    // MPI_Scatter(Y, part_size * MAXSIZE, MPI_INT, localY, part_size * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);

    int X_1D[MAXSIZE * MAXSIZE];
    int Y_1D[MAXSIZE * MAXSIZE];

    convert2Dto1D(X, X_1D);
    convert2Dto1D(Y, Y_1D);

    MPI_Scatter(X_1D, part_size, MPI_INT, localX_1D, part_size, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Scatter(Y_1D, part_size, MPI_INT, localY_1D, part_size, MPI_INT, root, MPI_COMM_WORLD);



/*parallelise here using OpenMP: fastest time wins!!!! Use clauses and anythign at your disposal. Change the code if you want to.
Consider NUMA, consider the chunk size of your schedules. Experiment!!!!!!!*/
// #pragma omp parallel
//     {
// #pragma omp for
//         // for (i = from; i < to; i++)
//         for (i = 0; i < part_size; i++)
//         {
//             for (j = 0; j < MAXSIZE; j++)
//             {
//                 localZ[i][j] = 0;
//                 for (k = 0; k < MAXSIZE; k++)
//                 {
//                     localZ[i][j] += localX[i][k] * localY[k][j];
//                 }
//             }
//         }
//     }

#pragma omp parallel
    {
#pragma omp for
        // for (i = from; i < to; i++)
        for (i = 0; i < part_size; i++)
        {
            localZ_1D[i] += localX_1D[i] * localY_1D[i];

            // for (j = 0; j < MAXSIZE; j++)
            // {
            //     localZ[i][j] = 0;
            //     for (k = 0; k < MAXSIZE; k++)
            //     {
            //         localZ[i][j] += localX[i][k] * localY[k][j];
            //     }
            // }
        }
    }

    // MPI_Gather(localZ, part_size * MAXSIZE, MPI_INT, Z, part_size * MAXSIZE, MPI_INT, root, MPI_COMM_WORLD);
    int Z_1D[MAXSIZE * MAXSIZE];
    MPI_Gather(localZ_1D, part_size, MPI_INT, Z_1D, part_size, MPI_INT, root, MPI_COMM_WORLD);
    convert1Dto2D(Z_1D, Z);

    /*if root print mat Z*/
    if (my_rank == 0)
    {
        print_matrix(Z);
    }

    tEnd = MPI_Wtime();

    printf("\nProgram took %lf milliseconds\n", (tEnd - tStart) * 1000);

    free(X_1D);
    free(Y_1D);
    free(Z_1D);

    MPI_Finalize();
    return 0;
}
