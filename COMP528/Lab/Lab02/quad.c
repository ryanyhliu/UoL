/*
 * example: quadrature
 * Written by Dr Maryam Abo-Tabik
 */

#include <stdio.h>
#include <math.h>
#include <sys/time.h>  // for wallclock timing functions

#define MAX(x,y) ( x>y ? x : y )

double func (double x);

int main(int argc, char** argv) {
  const double a=0.1, b=200.0;  /* bounds */
  const int numberQuads = 50000000;
  double integrand, width, x, y, meanHeight;
  int i;              /* loop counter */

  /* for timing */
  struct timeval wallStart, wallEnd;

  /* sequential in-line way */
  width = (b-a) / (float) numberQuads;
  integrand = 0.0;
  gettimeofday(&wallStart, NULL); // save start time in to variable 'wallStart'
  for (i=0; i<numberQuads; i++) {
    x = a + i*width;
    y = x + width;
    meanHeight = 0.5 * (func(x) + func(y));
    integrand += meanHeight*width;
    // printf("%d %.15f %f %f %f\n",i,x,y,meanHeight,integrand);
  }
  gettimeofday(&wallEnd, NULL); // end time
  printf("SERIAL- integral = %f\n", integrand);
  double wallSecs = (wallEnd.tv_sec - wallStart.tv_sec);           // just integral number of seconds
  double WALLtimeTaken = 1.0E-06 * ((wallSecs*1000000) + (wallEnd.tv_usec - wallStart.tv_usec)); // and now with any microseconds
  printf("WALL CLOCK Time: %f seconds  \n", WALLtimeTaken);
}