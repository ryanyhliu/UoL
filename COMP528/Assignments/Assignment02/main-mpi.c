
/**
 * TOMB FOR MY ORIGINAL IDEA
 * I THINK THIS CODE IS MORE REASONABLE THAN CURRENT VERSION
 * BUT I CANNOT FIX SEG FAULT IN MPI_FINALIZE();
 * WHICH IS MAYBE CAUSED BY STRUCTURE
 * I EVEN HAD GOT THE CORRECT OUTPUT FILES
*/


// typedef struct
// {
//     int *tour;
//     double totalDistance;
// } TourResult;



// int *findCheapestTour(double **dMatrix, int numOfCoords);
// int *findFarthestTour(double **dMatrix, int numOfCoords);
// int *findNearestTour(double **dMatrix, int numOfCoords);
// TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
// TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
// TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);


// int main(int argc, char *argv[])
// {
//     MPI_Init(&argc, &argv);

//     int world_size, world_rank;
//     MPI_Comm_size(MPI_COMM_WORLD, &world_size);
//     MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

//     if (argc < 5)
//     {
//         if (world_rank == 0)
//         {
//             fprintf(stderr, "Usage: %s <input file> <output file 1> <output file 2> <output file 3>\n", argv[0]);
//         }
//         MPI_Abort(MPI_COMM_WORLD, 1);
//         return 1;
//     }

//     printf("---TEST 01---\n");

//     char filename[500];
//     char outFileName_Cheapest[500];
//     char outFileName_Farthest[500];
//     char outFileName_Nearest[500];

//     strcpy(filename, argv[1]);
//     strcpy(outFileName_Cheapest, argv[2]);
//     strcpy(outFileName_Farthest, argv[3]);
//     strcpy(outFileName_Nearest, argv[4]);

//     int numOfCoords = readNumOfCoords(filename);
//     double **coords = readCoords(filename, numOfCoords);
//     double **dMatrix = createDistanceMatrix(coords, numOfCoords);

//     int tourLength = numOfCoords + 1;

//     printf("---TEST 02---\n");

//     // Define MPI data type for TourResult
//     MPI_Datatype MPI_TOURRESULT;
//     MPI_Datatype type[2] = {MPI_INT, MPI_DOUBLE};
//     int blocklen[2] = {tourLength, 1};
//     MPI_Aint disp[2];
//     disp[0] = offsetof(TourResult, tour);
//     disp[1] = offsetof(TourResult, totalDistance);
//     MPI_Type_create_struct(2, blocklen, disp, type, &MPI_TOURRESULT);
//     MPI_Type_commit(&MPI_TOURRESULT);

//     printf("---TEST 03---\n");

//     TourResult local_best_tour_cheapest = {
//         .tour = (int *)malloc(tourLength * sizeof(int)),
//         .totalDistance = __DBL_MAX__};

//     TourResult local_best_tour_farthest = {
//         .tour = (int *)malloc(tourLength * sizeof(int)),
//         .totalDistance = __DBL_MAX__};

//     TourResult local_best_tour_nearest = {
//         .tour = (int *)malloc(tourLength * sizeof(int)),
//         .totalDistance = __DBL_MAX__};

//     for (int i = 0; i < numOfCoords; i++)
//     {
//         TourResult current_tour_cheapest = cheapestInsertion(dMatrix, numOfCoords, i);
//         if (current_tour_cheapest.totalDistance < local_best_tour_cheapest.totalDistance)
//         {
//             memcpy(local_best_tour_cheapest.tour, current_tour_cheapest.tour, tourLength * sizeof(int));
//             local_best_tour_cheapest.totalDistance = current_tour_cheapest.totalDistance;
//         }
//         free(current_tour_cheapest.tour);
//         current_tour_cheapest.tour = NULL;

//         TourResult current_tour_farthest = farthestInsertion(dMatrix, numOfCoords, i);
//         if (current_tour_farthest.totalDistance < local_best_tour_farthest.totalDistance)
//         {
//             memcpy(local_best_tour_farthest.tour, current_tour_farthest.tour, tourLength * sizeof(int));
//             local_best_tour_farthest.totalDistance = current_tour_farthest.totalDistance;
//         }
//         free(current_tour_farthest.tour);
//         current_tour_farthest.tour = NULL;

//         TourResult current_tour_nearest = nearestAddition(dMatrix, numOfCoords, i);
//         if (current_tour_nearest.totalDistance < local_best_tour_nearest.totalDistance)
//         {
//             memcpy(local_best_tour_nearest.tour, current_tour_nearest.tour, tourLength * sizeof(int));
//             local_best_tour_nearest.totalDistance = current_tour_nearest.totalDistance;
//         }
//         free(current_tour_nearest.tour);
//         current_tour_nearest.tour = NULL;
//     }

//     printf("---TEST 04---\n");

//     // Gather results to the root process
//     TourResult *all_tours_cheapest = NULL, *all_tours_farthest = NULL, *all_tours_nearest = NULL;
//     if (world_rank == 0)
//     {
//         all_tours_cheapest = (TourResult *)malloc(world_size * sizeof(TourResult));
//         all_tours_farthest = (TourResult *)malloc(world_size * sizeof(TourResult));
//         all_tours_nearest = (TourResult *)malloc(world_size * sizeof(TourResult));

//         for (int i = 0; i < world_size; i++)
//         {
//             all_tours_cheapest[i].tour = (int *)malloc(tourLength * sizeof(int));
//             all_tours_farthest[i].tour = (int *)malloc(tourLength * sizeof(int));
//             all_tours_nearest[i].tour = (int *)malloc(tourLength * sizeof(int));
//         }
//     }

//     printf("---TEST 05---\n");

//     MPI_Gather(&local_best_tour_cheapest, 1, MPI_TOURRESULT, all_tours_cheapest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);
//     MPI_Gather(&local_best_tour_farthest, 1, MPI_TOURRESULT, all_tours_farthest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);
//     MPI_Gather(&local_best_tour_nearest, 1, MPI_TOURRESULT, all_tours_nearest, 1, MPI_TOURRESULT, 0, MPI_COMM_WORLD);

//     printf("---TEST 06---\n");

//     // Process results in the root process
//     if (world_rank == 0)
//     {
//         int minIndexCheapest = 0, minIndexFarthest = 0, minIndexNearest = 0;
//         double minDistanceCheapest = all_tours_cheapest[0].totalDistance;
//         double minDistanceFarthest = all_tours_farthest[0].totalDistance;
//         double minDistanceNearest = all_tours_nearest[0].totalDistance;

//         for (int i = 1; i < world_size; i++)
//         {
//             if (all_tours_cheapest[i].totalDistance < minDistanceCheapest)
//             {
//                 minDistanceCheapest = all_tours_cheapest[i].totalDistance;
//                 minIndexCheapest = i;
//             }
//             if (all_tours_farthest[i].totalDistance < minDistanceFarthest)
//             {
//                 minDistanceFarthest = all_tours_farthest[i].totalDistance;
//                 minIndexFarthest = i;
//             }
//             if (all_tours_nearest[i].totalDistance < minDistanceNearest)
//             {
//                 minDistanceNearest = all_tours_nearest[i].totalDistance;
//                 minIndexNearest = i;
//             }
//         }

//         printf("---TEST 07---\n");

//         // Write the best tours to their respective files
//         writeTourToFile(all_tours_cheapest[minIndexCheapest].tour, numOfCoords + 1, outFileName_Cheapest);
//         writeTourToFile(all_tours_farthest[minIndexFarthest].tour, numOfCoords + 1, outFileName_Farthest);
//         writeTourToFile(all_tours_nearest[minIndexNearest].tour, numOfCoords + 1, outFileName_Nearest);
//     }

//     printf("---TEST 08---\n");

//     // Clean up
//     MPI_Type_free(&MPI_TOURRESULT);

//     if (local_best_tour_cheapest.tour != NULL)
//     {
//         free(local_best_tour_cheapest.tour);
//         local_best_tour_cheapest.tour = NULL;
//     }
//     if (local_best_tour_farthest.tour != NULL)
//     {
//         free(local_best_tour_farthest.tour);
//         local_best_tour_farthest.tour = NULL;
//     }
//     if (local_best_tour_nearest.tour != NULL)
//     {
//         free(local_best_tour_nearest.tour);
//         local_best_tour_nearest.tour = NULL;
//     }

//     printf("---TEST 09---\n");

//     if (world_rank == 0) {
//         for (int i = 0; i < world_size; i++) {
//             // 释放每个结构体中的tour数组
//             if (all_tours_cheapest[i].tour != NULL) {
//                 free(all_tours_cheapest[i].tour);
//                 all_tours_cheapest[i].tour = NULL;
//             }
//             printf("---TEST 091---\n");

//             if (all_tours_farthest[i].tour != NULL) {
//                 free(all_tours_farthest[i].tour);
//                 all_tours_farthest[i].tour = NULL;
//             }
//             printf("---TEST 092---\n");

//             if (all_tours_nearest[i].tour != NULL) {
//                 free(all_tours_nearest[i].tour);
//                 all_tours_nearest[i].tour = NULL;
//             }
//             printf("---TEST 093---\n");
//         }

//         //释放结构体数组
//         // if (all_tours_cheapest != NULL)
//         // {
//         //     free(all_tours_cheapest);
//         // }
//         // printf("---TEST 094---\n");

//         // if (all_tours_farthest != NULL)
//         // {
//         //     free(all_tours_farthest);
//         // }
//         // printf("---TEST 095---\n");

//         // if (all_tours_nearest != NULL)
//         // {
//         //     free(all_tours_nearest);
//         // }

//     }

//     printf("---TEST 10---\n");

//     // for (int i = 0; i < numOfCoords; i++) {
//     //     free(coords[i]);
//     //     free(dMatrix[i]);
//     // }

//     free(coords);
//     coords = NULL;
//     free(dMatrix);
//     dMatrix = NULL;

//     printf("---TEST 11---\n");

//     MPI_Finalize();
//     printf("---TEST 12---\n");

//     return 0;
// }



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include <mpi.h>
#include <stddef.h>




// 定义一个结构体来存储距离和对应的索引
typedef struct {
    double distance;
    int index;
} DistanceIndex;











int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
double **createDistanceMatrix(double **coords, int numOfCoords);
double sqrt(double arg);

double getDistance_CheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *getTour_CheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
double getDistance_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
double getDistance_NearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *getTour_NearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);







// 自定义的归约操作，用于同时更新距离和索引
void minDistanceIndex(DistanceIndex *in, DistanceIndex *inout, int *len, MPI_Datatype *datatype) {
    for (int i = 0; i < *len; ++i) {
        if (in[i].distance < inout[i].distance) {
            inout[i].distance = in[i].distance;
            inout[i].index = in[i].index;
        }
    }
}





int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (argc < 5) {
        if (world_rank == 0) {
            fprintf(stderr, "Usage: %s <input file> <output file 1> <output file 2> <output file 3>\n", argv[0]);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    char filename[500];
    char outFileName_Cheapest[500];
    char outFileName_Farthest[500];
    char outFileName_Nearest[500];

    strcpy(filename, argv[1]);
    strcpy(outFileName_Cheapest, argv[2]);
    strcpy(outFileName_Farthest, argv[3]);
    strcpy(outFileName_Nearest, argv[4]);

    int numOfCoords = readNumOfCoords(filename);
    double **coords = readCoords(filename, numOfCoords);
    double **dMatrix = createDistanceMatrix(coords, numOfCoords);

    // 分布式计算最佳起点
    DistanceIndex localMinCheapest = {__DBL_MAX__, -1};
    DistanceIndex localMinNearest = {__DBL_MAX__, -1};
    
    double localMinDistanceFarthest = __DBL_MAX__;
    int localBestStartPointFarthest = -1;


    bool hasAssignedTask = false;
    // int processedCoords = 0;  // 新增变量，记录分配的任务总数

    for (int i = world_rank; i < numOfCoords; i += world_size) {
        hasAssignedTask = true;
        // processedCoords++;  // 为每个处理的坐标增加计数

        // ... (计算最佳起点的循环) ...
        double distanceCheapest = getDistance_CheapestInsertion(dMatrix, numOfCoords, i);
        if (distanceCheapest < localMinCheapest.distance) {
            localMinCheapest.distance = distanceCheapest;
            localMinCheapest.index = i;
        }

        double distanceNearest = getDistance_NearestAddition(dMatrix, numOfCoords, i);
        if (distanceNearest < localMinNearest.distance) {
            localMinNearest.distance = distanceNearest;
            localMinNearest.index = i;
        }

        // 保持原有FarthestInsertion逻辑
        double distanceFarthest = getDistance_FarthestInsertion(dMatrix, numOfCoords, i);
        if (distanceFarthest < localMinDistanceFarthest) {
            localMinDistanceFarthest = distanceFarthest;
            localBestStartPointFarthest = i;
        }
    }

    // 重新计算processedCoords，确保它包含实际处理的坐标数
    // processedCoords = (numOfCoords / world_size) + (numOfCoords % world_size > world_rank ? 1 : 0);

    

    if (!hasAssignedTask) {
        // 处理剩余的坐标
        for (int i = world_rank; i < numOfCoords; i += world_size) {
            // 注意，这里的循环开始点和步长与主循环相同，但是起始索引进行了调整

            // 再次执行相同的计算，对于这些剩余的坐标
            double distanceCheapest = getDistance_CheapestInsertion(dMatrix, numOfCoords, i);
            if (distanceCheapest < localMinCheapest.distance) {
                localMinCheapest.distance = distanceCheapest;
                localMinCheapest.index = i;
            }

            double distanceNearest = getDistance_NearestAddition(dMatrix, numOfCoords, i);
            if (distanceNearest < localMinNearest.distance) {
                localMinNearest.distance = distanceNearest;
                localMinNearest.index = i;
            }

            double distanceFarthest = getDistance_FarthestInsertion(dMatrix, numOfCoords, i);
            if (distanceFarthest < localMinDistanceFarthest) {
                localMinDistanceFarthest = distanceFarthest;
                localBestStartPointFarthest = i;
            }
        }
    }


    // 汇总全局最佳起点
    DistanceIndex globalMinCheapest, globalMinNearest;
    double globalMinDistanceFarthest;
    int globalBestStartPointFarthest;

    MPI_Op customOp;
    MPI_Op_create((MPI_User_function *)minDistanceIndex, 1, &customOp);

    MPI_Allreduce(&localMinCheapest, &globalMinCheapest, 1, MPI_DOUBLE_INT, customOp, MPI_COMM_WORLD);
    MPI_Allreduce(&localMinNearest, &globalMinNearest, 1, MPI_DOUBLE_INT, customOp, MPI_COMM_WORLD);

    // 使用原始的MPI逻辑对FarthestInsertion进行汇总
    MPI_Allreduce(&localMinDistanceFarthest, &globalMinDistanceFarthest, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(&localBestStartPointFarthest, &globalBestStartPointFarthest, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    MPI_Op_free(&customOp);

    // 主进程使用最佳起点获取完整路径并输出
    if (world_rank == 0) {
        int *tourCheapest = getTour_CheapestInsertion(dMatrix, numOfCoords, globalMinCheapest.index);
        int *tourFarthest = getTour_FarthestInsertion(dMatrix, numOfCoords, globalBestStartPointFarthest);
        int *tourNearest = getTour_NearestAddition(dMatrix, numOfCoords, globalMinNearest.index);

        writeTourToFile(tourCheapest, numOfCoords + 1, outFileName_Cheapest);
        writeTourToFile(tourFarthest, numOfCoords + 1, outFileName_Farthest);
        writeTourToFile(tourNearest, numOfCoords + 1, outFileName_Nearest);

        free(tourCheapest);
        free(tourFarthest);
        free(tourNearest);
    }

    // 清理和MPI终止
    for (int i = 0; i < numOfCoords; i++) {
        free(coords[i]);
    }
    free(coords);

    for (int i = 0; i < numOfCoords; i++) {
        free(dMatrix[i]);
    }
    free(dMatrix);

    MPI_Finalize();
    return 0;
}



























// int main(int argc, char *argv[])
// {
//     MPI_Init(&argc, &argv);

//     int world_size, world_rank;
//     MPI_Comm_size(MPI_COMM_WORLD, &world_size);
//     MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

//     // ... 文件读取和数据初始化 ...
//     if (argc < 5)
//     {
//         if (world_rank == 0)
//         {
//             fprintf(stderr, "Usage: %s <input file> <output file 1> <output file 2> <output file 3>\n", argv[0]);
//         }
//         MPI_Abort(MPI_COMM_WORLD, 1);
//         return 1;
//     }

//     char filename[500];
//     char outFileName_Cheapest[500];
//     char outFileName_Farthest[500];
//     char outFileName_Nearest[500];

//     strcpy(filename, argv[1]);
//     strcpy(outFileName_Cheapest, argv[2]);
//     strcpy(outFileName_Farthest, argv[3]);
//     strcpy(outFileName_Nearest, argv[4]);

//     int numOfCoords = readNumOfCoords(filename);
//     double **coords = readCoords(filename, numOfCoords);
//     double **dMatrix = createDistanceMatrix(coords, numOfCoords);

//     // 分布式计算最佳起点
//     double localMinDistanceCheapest = __DBL_MAX__;
//     double localMinDistanceFarthest = __DBL_MAX__;
//     double localMinDistanceNearest = __DBL_MAX__;

//     int localBestStartPointCheapest = -1;
//     int localBestStartPointFarthest = -1;
//     int localBestStartPointNearest = -1;

//     for (int i = world_rank; i < numOfCoords; i += world_size)
//     {
//         double distanceCheapest = getDistance_CheapestInsertion(dMatrix, numOfCoords, i);
//         if (distanceCheapest < localMinDistanceCheapest)
//         {
//             localMinDistanceCheapest = distanceCheapest;
//             localBestStartPointCheapest = i;
//         }

//         double distanceFarthest = getDistance_FarthestInsertion(dMatrix, numOfCoords, i);
//         if (distanceFarthest < localMinDistanceFarthest)
//         {
//             localMinDistanceFarthest = distanceFarthest;
//             localBestStartPointFarthest = i;
//         }

//         double distanceNearest = getDistance_NearestAddition(dMatrix, numOfCoords, i);
//         if (distanceNearest < localMinDistanceNearest)
//         {
//             localMinDistanceNearest = distanceNearest;
//             localBestStartPointNearest = i;
//         }
//     }

//     // 汇总全局最佳起点
//     double globalMinDistanceCheapest, globalMinDistanceFarthest, globalMinDistanceNearest;
//     int globalBestStartPointCheapest, globalBestStartPointFarthest, globalBestStartPointNearest;

//     MPI_Allreduce(&localMinDistanceCheapest, &globalMinDistanceCheapest, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
//     MPI_Allreduce(&localBestStartPointCheapest, &globalBestStartPointCheapest, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

//     MPI_Allreduce(&localMinDistanceFarthest, &globalMinDistanceFarthest, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
//     MPI_Allreduce(&localBestStartPointFarthest, &globalBestStartPointFarthest, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

//     MPI_Allreduce(&localMinDistanceNearest, &globalMinDistanceNearest, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
//     MPI_Allreduce(&localBestStartPointNearest, &globalBestStartPointNearest, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

//     // 主进程使用最佳起点获取完整路径并输出
//     if (world_rank == 0)
//     {
//         int *tourCheapest = getTour_CheapestInsertion(dMatrix, numOfCoords, globalBestStartPointCheapest);
//         int *tourFarthest = getTour_FarthestInsertion(dMatrix, numOfCoords, globalBestStartPointFarthest);
//         int *tourNearest = getTour_NearestAddition(dMatrix, numOfCoords, globalBestStartPointNearest);

//         // ... 将tourCheapest, tourFarthest, tourNearest写入相应的文件 ...
//         // Write the best tours to their respective files
//         writeTourToFile(tourCheapest, numOfCoords + 1, outFileName_Cheapest);
//         writeTourToFile(tourFarthest, numOfCoords + 1, outFileName_Farthest);
//         writeTourToFile(tourNearest, numOfCoords + 1, outFileName_Nearest);
    

//         free(tourCheapest);
//         free(tourFarthest);
//         free(tourNearest);
//     }

//     // ... 清理和MPI终止 ...
//     free(coords);
//     free(dMatrix);

//     MPI_Finalize();
//     return 0;
// }

double **createDistanceMatrix(double **coords, int numOfCoords)
{
    int i, j;

    double **dMatrix = (double **)malloc(numOfCoords * sizeof(double));

    for (i = 0; i < numOfCoords; i++)
    {
        dMatrix[i] = (double *)malloc(numOfCoords * sizeof(double));
    }

#pragma omp parallel for collapse(2)
    for (i = 0; i < numOfCoords; i++)
    {
        for (j = 0; j < numOfCoords; j++)
        {
            double diffX = coords[i][0] - coords[j][0];
            double diffY = coords[i][1] - coords[j][1];
            dMatrix[i][j] = sqrt((diffX * diffX) + (diffY * diffY));
        }
    }

    return dMatrix;
}
