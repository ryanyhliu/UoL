#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>

// typedef struct {
//     int *tour;
//     double totalDistance;
// } TourResult;



double getDistance_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd)
{
    double tolerance = 1e-9;
    double totalDistance = 0.0;

    int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    // 初始化路径和访问状态
    for (int i = 0; i < numOfCoords; i++) {
        tour[i] = -1;
        visited[i] = false;
    }

    // 起始点加入路径并标记为已访问
    tour[0] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;
    int tourLength = 1;

    while (tourLength < numOfCoords) {
        double maxCost = 0;
        int nextNode = -1;

        // 并行寻找最远的未访问节点
        #pragma omp parallel for reduction(max:maxCost)
        for (int i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                for (int j = 0; j < tourLength; j++) {
                    double cost = dMatrix[tour[j]][i];
                    if (cost > maxCost) {
                        #pragma omp critical
                        {
                            if (cost > maxCost) {
                                maxCost = cost;
                                nextNode = i;
                            }
                        }
                    }
                }
            }
        }

        double minCost = __DBL_MAX__;
        int insertPos = -1;

        // 并行寻找插入最远节点的最佳位置
        #pragma omp parallel for reduction(min:minCost)
        for (int i = 0; i < tourLength; i++) {
            int k = (i + 1) % tourLength;
            double cost = dMatrix[tour[i]][nextNode] + dMatrix[nextNode][tour[k]] - dMatrix[tour[i]][tour[k]];
            if (cost < minCost - tolerance) {
                #pragma omp critical
                {
                    if (cost < minCost - tolerance) {
                        minCost = cost;
                        insertPos = i + 1;
                    }
                }
            }
        }

        // 插入节点
        for (int i = tourLength; i >= insertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[insertPos] = nextNode;
        visited[nextNode] = true;
        tourLength++;
        totalDistance += minCost;
    }

    // 将起始点加到路径末尾，形成闭环
    tour[numOfCoords] = pointOfStartEnd;

    free(visited);
    free(tour)

    return totalDistance;
}


int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd) {
    double tolerance = 1e-9;
    double totalDistance = 0.0;

    int *tour = (int *)malloc((numOfCoords + 1) * sizeof(int));
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

    // 初始化路径和访问状态
    for (int i = 0; i < numOfCoords; i++) {
        tour[i] = -1;
        visited[i] = false;
    }

    // 起始点加入路径并标记为已访问
    tour[0] = pointOfStartEnd;
    visited[pointOfStartEnd] = true;
    int tourLength = 1;

    while (tourLength < numOfCoords) {
        double maxCost = 0;
        int nextNode = -1;

        // 并行寻找最远的未访问节点
        #pragma omp parallel for reduction(max:maxCost)
        for (int i = 0; i < numOfCoords; i++) {
            if (!visited[i]) {
                for (int j = 0; j < tourLength; j++) {
                    double cost = dMatrix[tour[j]][i];
                    if (cost > maxCost) {
                        #pragma omp critical
                        {
                            if (cost > maxCost) {
                                maxCost = cost;
                                nextNode = i;
                            }
                        }
                    }
                }
            }
        }

        double minCost = __DBL_MAX__;
        int insertPos = -1;

        // 并行寻找插入最远节点的最佳位置
        #pragma omp parallel for reduction(min:minCost)
        for (int i = 0; i < tourLength; i++) {
            int k = (i + 1) % tourLength;
            double cost = dMatrix[tour[i]][nextNode] + dMatrix[nextNode][tour[k]] - dMatrix[tour[i]][tour[k]];
            if (cost < minCost - tolerance) {
                #pragma omp critical
                {
                    if (cost < minCost - tolerance) {
                        minCost = cost;
                        insertPos = i + 1;
                    }
                }
            }
        }

        // 插入节点
        for (int i = tourLength; i >= insertPos; i--) {
            tour[i] = tour[i - 1];
        }
        tour[insertPos] = nextNode;
        visited[nextNode] = true;
        tourLength++;
        totalDistance += minCost;
    }

    // 将起始点加到路径末尾，形成闭环
    tour[numOfCoords] = pointOfStartEnd;

    free(visited);
    return tour;
}

















// double getDistance_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd)
// {
//     double totalDistance = 0.0;

//     // Setting up variables
//     int nextNode, insertPos;

//     // Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
//     // Visited uses calloc, array is instantiated with "0" as all elements. Good for boolean arrays.
//     int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
//     bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

//     // Initialising tour to empty
//     for (int i = 0; i < numOfCoords; i++)
//     {
//         tour[i] = -1;
//     }

//     // Tour always starts with 0. 0 is visited
//     tour[0] = pointOfStartEnd;
//     tour[1] = pointOfStartEnd;
//     visited[pointOfStartEnd] = true;

//     // Hard coding because I'm lazy
//     int numVisited = 1;
//     int tourLength = 2;

//     // Where OMP starts... Get the env variable for the max num of threads.
//     int numThreads = omp_get_max_threads();

//     /*
//     Set up arrays to be the size of the number of threads. Each thread will store
//     its minCost, its nextNode, and its insertPos in its respective memory location.
//     Thread 0 will store its results at position 0, thread 1 will store its results at position 1 etc.
//     Multiply by 8 to avoid false sharing. Each space is 64 bytes long (to ensure each thread has its own cache line)
//     */

//     double *threadMinCosts = NULL;
//     double *threadMaxCosts = NULL;
//     int *threadNextNode = NULL;
//     int *threadInsertPos = NULL;

//     threadMinCosts = (double *)malloc(numThreads * 8 * sizeof(double));
//     threadMaxCosts = (double *)malloc(numThreads * 8 * sizeof(double));
//     threadNextNode = (int *)malloc(numThreads * 8 * sizeof(int));
//     threadInsertPos = (int *)malloc(numThreads * 8 * sizeof(int));

//     int bestNextNode = -1;

// // Start a parallel section
// #pragma omp parallel
//     {

//         // Each thread now has started, and it stores its thread number in threadID
//         int threadID = omp_get_thread_num();

//         while (numVisited < numOfCoords)
//         {

//             // Point 1: Thread only accesses its memory location in the shared array.
//             threadMinCosts[threadID * 8] = ________DBL_MAX________;
//             threadMaxCosts[threadID * 8] = 0;
//             threadNextNode[threadID * 8] = -1;
//             threadInsertPos[threadID * 8] = -1;

// // Begin a workshare construct. Threads divide i and j and work on their respective ones.
// #pragma omp for collapse(2)
//             for (int i = 0; i < tourLength - 1; i++)
//             {
//                 for (int j = 0; j < numOfCoords; j++)
//                 {
//                     // Each thread identifies their farthest vertex from a vertex in the tour
//                     if (!visited[j])
//                     {
//                         double cost = dMatrix[tour[i]][j];
//                         if (cost > threadMaxCosts[threadID * 8])
//                         {
//                             // See Point 1
//                             threadMaxCosts[threadID * 8] = cost;
//                             threadNextNode[threadID * 8] = j;
//                         }
//                     }
//                 }
//             }

// // Single construct, one thread looks through what each thread has. Choosest the farthest node.
// #pragma omp single
//             {
//                 double maxCost = 0;
//                 for (int i = 0; i < numThreads; i++)
//                 {
//                     if (threadMaxCosts[i * 8] > maxCost)
//                     {
//                         maxCost = threadMaxCosts[i * 8];
//                         bestNextNode = threadNextNode[i * 8];
//                     }
//                 }
//             }

// // Find the cost of adding the farthest node to every possible location in the tour. Each thread finds their own.
// #pragma omp for
//             for (int k = 0; k < tourLength - 1; k++)
//             {
//                 double cost = dMatrix[tour[k]][bestNextNode] + dMatrix[bestNextNode][tour[k + 1]] - dMatrix[tour[k]][tour[k + 1]];
//                 if (cost < threadMinCosts[threadID * 8])
//                 {
//                     threadMinCosts[threadID * 8] = cost;
//                     threadInsertPos[threadID * 8] = k + 1;
//                 }
//             }

// // Single construct only one thread working on this part.
// #pragma omp single
//             {
//                 int bestInsertPos = -1;
//                 double minCost = ________DBL_MAX________;

//                 // Single thread loops through every thread's answer and chooses the cheapest one.
//                 for (int i = 0; i < numThreads; i++)
//                 {
//                     if (threadMinCosts[i * 8] < minCost)
//                     {
//                         minCost = threadMinCosts[i * 8];
//                         bestInsertPos = threadInsertPos[i * 8];
//                     }
//                 }

//                 // Single thread places the bestNextNode in the bestInsertPos
//                 for (int i = numOfCoords; i > bestInsertPos; i--)
//                 {
//                     tour[i] = tour[i - 1];
//                 }

//                 tour[bestInsertPos] = bestNextNode;
//                 visited[bestNextNode] = true;

//                 tourLength++;
//                 numVisited++;

//                 totalDistance += minCost;
//             }
//         }
//     }

//     // Free all memory when done

//     free(visited);
//     free(threadMinCosts);
//     free(threadNextNode);
//     free(threadInsertPos);
//     free(threadMaxCosts);
//     free(tour);

//     return totalDistance;
// }

// int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd)
// {
//     // Setting up variables
//     int nextNode, insertPos;

//     // Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
//     // Visited uses calloc, array is instantiated with "0" as all elements. Good for boolean arrays.
//     int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
//     bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));

//     // Initialising tour to empty
//     for (int i = 0; i < numOfCoords; i++)
//     {
//         tour[i] = -1;
//     }

//     // Tour always starts with 0. 0 is visited
//     tour[0] = pointOfStartEnd;
//     tour[1] = pointOfStartEnd;
//     visited[pointOfStartEnd] = true;

//     // Hard coding because I'm lazy
//     int numVisited = 1;
//     int tourLength = 2;

//     // Where OMP starts... Get the env variable for the max num of threads.
//     int numThreads = omp_get_max_threads();

//     /*
//     Set up arrays to be the size of the number of threads. Each thread will store
//     its minCost, its nextNode, and its insertPos in its respective memory location.
//     Thread 0 will store its results at position 0, thread 1 will store its results at position 1 etc.
//     Multiply by 8 to avoid false sharing. Each space is 64 bytes long (to ensure each thread has its own cache line)
//     */

//     double *threadMinCosts = NULL;
//     double *threadMaxCosts = NULL;
//     int *threadNextNode = NULL;
//     int *threadInsertPos = NULL;

//     threadMinCosts = (double *)malloc(numThreads * 8 * sizeof(double));
//     threadMaxCosts = (double *)malloc(numThreads * 8 * sizeof(double));
//     threadNextNode = (int *)malloc(numThreads * 8 * sizeof(int));
//     threadInsertPos = (int *)malloc(numThreads * 8 * sizeof(int));

//     int bestNextNode = -1;

// // Start a parallel section
// #pragma omp parallel
//     {

//         // Each thread now has started, and it stores its thread number in threadID
//         int threadID = omp_get_thread_num();

//         while (numVisited < numOfCoords)
//         {

//             // Point 1: Thread only accesses its memory location in the shared array.
//             threadMinCosts[threadID * 8] = ________DBL_MAX________;
//             threadMaxCosts[threadID * 8] = 0;
//             threadNextNode[threadID * 8] = -1;
//             threadInsertPos[threadID * 8] = -1;

// // Begin a workshare construct. Threads divide i and j and work on their respective ones.
// #pragma omp for collapse(2)
//             for (int i = 0; i < tourLength - 1; i++)
//             {
//                 for (int j = 0; j < numOfCoords; j++)
//                 {
//                     // Each thread identifies their farthest vertex from a vertex in the tour
//                     if (!visited[j])
//                     {
//                         double cost = dMatrix[tour[i]][j];
//                         if (cost > threadMaxCosts[threadID * 8])
//                         {
//                             // See Point 1
//                             threadMaxCosts[threadID * 8] = cost;
//                             threadNextNode[threadID * 8] = j;
//                         }
//                     }
//                 }
//             }

// // Single construct, one thread looks through what each thread has. Choosest the farthest node.
// #pragma omp single
//             {
//                 double maxCost = 0;
//                 for (int i = 0; i < numThreads; i++)
//                 {
//                     if (threadMaxCosts[i * 8] > maxCost)
//                     {
//                         maxCost = threadMaxCosts[i * 8];
//                         bestNextNode = threadNextNode[i * 8];
//                     }
//                 }
//             }

// // Find the cost of adding the farthest node to every possible location in the tour. Each thread finds their own.
// #pragma omp for
//             for (int k = 0; k < tourLength - 1; k++)
//             {
//                 double cost = dMatrix[tour[k]][bestNextNode] + dMatrix[bestNextNode][tour[k + 1]] - dMatrix[tour[k]][tour[k + 1]];
//                 if (cost < threadMinCosts[threadID * 8])
//                 {
//                     threadMinCosts[threadID * 8] = cost;
//                     threadInsertPos[threadID * 8] = k + 1;
//                 }
//             }

// // Single construct only one thread working on this part.
// #pragma omp single
//             {
//                 int bestInsertPos = -1;
//                 double minCost = ________DBL_MAX________;

//                 // Single thread loops through every thread's answer and chooses the cheapest one.
//                 for (int i = 0; i < numThreads; i++)
//                 {
//                     if (threadMinCosts[i * 8] < minCost)
//                     {
//                         minCost = threadMinCosts[i * 8];
//                         bestInsertPos = threadInsertPos[i * 8];
//                     }
//                 }

//                 // Single thread places the bestNextNode in the bestInsertPos
//                 for (int i = numOfCoords; i > bestInsertPos; i--)
//                 {
//                     tour[i] = tour[i - 1];
//                 }

//                 tour[bestInsertPos] = bestNextNode;
//                 visited[bestNextNode] = true;

//                 tourLength++;
//                 numVisited++;
//             }
//         }
//     }

//     // Free all memory when done

//     free(visited);
//     free(threadMinCosts);
//     free(threadNextNode);
//     free(threadInsertPos);
//     free(threadMaxCosts);

//     return tour;
// }