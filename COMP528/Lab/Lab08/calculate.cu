#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>

#define MAX_NUM 100

__global__
void printNums(){
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    if (index <= MAX_NUM){
        int square = index * index;
        if (square <= MAX_NUM){
            printf("%d ^ 2 = %d\n", index, square);
        }
    }
}

int main(){
    int threadsPerBlk = 256;
    int blocks = (MAX_NUM + threadsPerBlk - 1) / threadsPerBlk;

    printNums<<<blocks, threadsPerBlk>>>();
    cudaDeviceSynchronize();

    return 0;
}



