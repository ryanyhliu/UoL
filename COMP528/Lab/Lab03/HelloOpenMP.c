#include <stdio.h>
#include <omp.h>


//double avg(int x[])
//{
//    int sum = 0;
//    int count = 0;
//
//    //int length = sizeof(x) / sizeof(x[0]);
//    int length = 10;
//    count = length;
//    int i = 0;
//    for (; i < length; i++)
//    {
//        sum += x[i];
//    }
//
//    return sum / count;
//}
//
//double min(int x[])
//{
//    double minValue = 1000;
//
//    //int length = sizeof(x) / sizeof(x[0]);
//    int length = 10;
//    int i = 0;
//    for (; i < length; i++)
//    {
//        if (minValue >= x[i])
//        {
//            minValue = x[i];
//        }
//    }
//
//    return minValue;
//}
//
//double max(int x[])
//{
//    double maxValue = 0;
//
//    //int length = sizeof(x) / sizeof(x[0]);
//    int length = 10;
//    int i = 0;
//    for (; i < length; i++)
//    {
//        if (maxValue <= x[i])
//        {
//            maxValue = x[i];
//        }
//    }
//
//    return maxValue;
//}


int main(int argc, char const* argv[]) {

#pragma omp parallel
    {
        int nthreads, thread_id;
        nthreads = omp_get_num_threads();
        thread_id = omp_get_thread_num();
        printf("Goodbye slow serial world and Hello OpenMP!\n");
        printf("I have %d thread(s) and my thread id is %d\n", nthreads, thread_id);
    }


//    double avgResult, minResult, maxResult, result;
//    int x[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
//#pragma omp parallel num_threads(40)
//    {
//        int num = omp_get_thread_num();
//        if (num == 0)
//        {
//           avgResult = avg(x);
//           printf("AVG: %d\n", avgResult);
//        }
//        else if (num == 1)
//        {
//            minResult = min(x);
//            printf("MIN: %d\n", minResult);
//        }
//        else if (num == 2)
//        {
//            maxResult = max(x);
//            printf("MAX: %d\n", maxResult);
//        }
//    }
//    result = avgResult + minResult + maxResult;
//
//    printf("result is %d\n", result);


    return 0;
}
