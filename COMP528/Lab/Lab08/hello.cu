#include <stdio.h>
#include <stdlib.h>

__global__
void helloKernel(int N)
{
  int i = threadIdx.x + blockIdx.x*blockDim.x;

  if (i<N) {
    printf("thread %d of block %d (dim: %d): iter %d of %d\n", threadIdx.x, blockIdx.x, blockDim.x, i, N);
  }
}


int main(void) {
  int N=20,numGPUs;
  int blks=5;
  int threadsPerBlk=4;

  /* check have GPU else quit */
  cudaGetDeviceCount(&numGPUs);
  printf("GPU nums: %d \n", numGPUs);


  if (numGPUs >= 1 ) {

    printf("hello on CPU\n");

    /* call GPU kernel  */
    helloKernel<<<blks,threadsPerBlk>>> (N);
    // cudaDeviceSynchronize();


    printf("hello again from CPU\n");


  }else {
    printf("no GPU present\n");
  }

}
