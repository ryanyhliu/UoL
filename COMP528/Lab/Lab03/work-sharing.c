#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5

int main (int argc, char *argv[]) {
  int nthreads, thread_id;
  
  int i;
  float a[N], b[N], c[N];
  double start, finish;
  
  for (i=0; i < N; i++)
    a[i] = b[i] = i; // initialize arrays
  
  printf("..............................................................\n");
  start=omp_get_wtime();
  printf("starting work block 1");
  #pragma omp parallel shared(a,b,c,nthreads) private(i,thread_id)
  {
    thread_id = omp_get_thread_num();
    if (thread_id == 0)
    {
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }
    printf("Thread %d starting...\n",thread_id);
    
    for (i=0; i<N; i++){
      c[i] = a[i] + b[i];
      printf("Thread %d: c[%d]= %f\n",thread_id,i,c[i]);
      }
  } 
  finish=omp_get_wtime();
  printf("work block 1 time: %f seconds\n",finish-start);
  
  printf("******************************************************\n");
  start=omp_get_wtime();
  printf("starting work block 2");
  #pragma omp parallel shared(a,b,c,nthreads) private(i,thread_id)
  {
    thread_id = omp_get_thread_num();
    if (thread_id == 0)
    {
      nthreads = omp_get_num_threads();
      printf("Number of threads = %d\n", nthreads);
    }
    #pragma omp for 
    for (i=0; i<N; i++){
      c[i] = a[i] + b[i];
      printf("Thread %d: c[%d]= %f\n",thread_id,i,c[i]);
      }
  } 
  finish=omp_get_wtime();
  printf("work block 2 time: %f seconds\n",finish-start); 
 } 