#include<stdio.h>
#include<stdlib.h>
#include<cuda_runtime.h>


__global__ void vectorArithmetic(float *z, const float *x, const float *y, float A, int num) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    if (i < num) {
        z[i] = A * x[i] + y[i];
    }
}


int main(void){
	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);



	//declaration of host variables
	const int num = 1000000;
	float *z, *x, *y;


	
	float A = 34;
	
	//initialising host variables
	z = (float*) malloc(num * sizeof(float));
	x = (float*) malloc(num * sizeof(float));
	y = (float*) malloc(num * sizeof(float));
	
	for(int i = 0; i < num; i++){
		x[i] = i;
		y[i] = 7 * i;
	}

	// //task to be performed by GPU. Don't forget to add global
	// for(int i = 0; i < num; i++){
	// 	z[i] = A * x[i] + y[i];
	// }
	


	float *d_x, *d_y, *d_z;
	cudaMalloc((void **)&d_x, num * sizeof(float));
	cudaMalloc((void **)&d_y, num * sizeof(float));
	cudaMalloc((void **)&d_z, num * sizeof(float));

	cudaMemcpy(d_x, x, num * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_y, y, num * sizeof(float), cudaMemcpyHostToDevice);

	int blockSize = 512; // 可以根据需要调整
	int numBlocks = (num + blockSize - 1) / blockSize;

	cudaEventRecord(start);
	vectorArithmetic<<<numBlocks, blockSize>>>(d_z, d_x, d_y, A, num);
	cudaEventRecord(stop);

	cudaMemcpy(z, d_z, num * sizeof(float), cudaMemcpyDeviceToHost);

	//postprocessing: output to terminal
	for(int j = 0; j < num; j++){
		printf("%f ", z[j]);
	}

	cudaFree(d_x);
	cudaFree(d_y);
	cudaFree(d_z);

	float milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);
	printf("Elapsed time: %f ms\n", milliseconds);

	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	
	return 0;
}
