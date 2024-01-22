#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<omp.h>
#include<string.h>

// typedef struct {
//     int *tour;
//     double totalDistance;
// } TourResult;



int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
double **createDistanceMatrix(double **coords, int numOfCoords);
double sqrt(double arg);

// int *findCheapestTour(double **dMatrix, int numOfCoords);
// int *findFarthestTour(double **dMatrix, int numOfCoords);
// int *findNearestTour(double **dMatrix, int numOfCoords);
// TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
// TourResult farthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
// TourResult nearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);

double getDistance_CheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *getTour_CheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
double getDistance_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *getTour_FarthestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
double getDistance_NearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *getTour_NearestAddition(double **dMatrix, int numOfCoords, int pointOfStartEnd);


int main(int argc, char *argv[]){
    double tolerance = 1e-9;

    if (argc != 5){
        printf("Error: Incorrect number of arguments. Expected 4 file names.\n");
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

    double tStart = omp_get_wtime();

    double minDistanceCheapest = __DBL_MAX__;
    double minDistanceFarthest = __DBL_MAX__;
    double minDistanceNearest = __DBL_MAX__;
    int *tourCheapest = NULL;
    int *tourFarthest = NULL;
    int *tourNearest = NULL;

    // 寻找最佳起点并生成对应路径
    for (int i = 0; i < numOfCoords; i++) {
        int *tempTourCheapest = getTour_CheapestInsertion(dMatrix, numOfCoords, i);
        double distanceCheapest = getDistance_CheapestInsertion(dMatrix, numOfCoords, i); 
        if (distanceCheapest < minDistanceCheapest  - tolerance) {
            minDistanceCheapest = distanceCheapest;
            free(tourCheapest);  // 释放旧路径
            tourCheapest = tempTourCheapest;
        } else {
            free(tempTourCheapest);
        }

        int *tempTourFarthest = getTour_FarthestInsertion(dMatrix, numOfCoords, i);
        double distanceFarthest = getDistance_FarthestInsertion(dMatrix, numOfCoords, i); 
        if (distanceFarthest < minDistanceFarthest  - tolerance) {
            minDistanceFarthest = distanceFarthest;
            free(tourFarthest);  // 释放旧路径
            tourFarthest = tempTourFarthest;
        } else {
            free(tempTourFarthest);
        }

        int *tempTourNearest = getTour_NearestAddition(dMatrix, numOfCoords, i);
        double distanceNearest = getDistance_NearestAddition(dMatrix, numOfCoords, i); 
        if (distanceNearest < minDistanceNearest  - tolerance) {
            minDistanceNearest = distanceNearest;
            free(tourNearest);  // 释放旧路径
            tourNearest = tempTourNearest;
        } else {
            free(tempTourNearest);
        }
    }

    double tEnd = omp_get_wtime();
    printf("Took %f milliseconds\n", (tEnd - tStart) * 1000);

    // 写入计算出的路径到指定的输出文件
    if (writeTourToFile(tourCheapest, numOfCoords + 1, outFileName_Cheapest) == NULL){
        printf("Error writing Cheapest Insertion Tour\n");
    }
    if (writeTourToFile(tourFarthest, numOfCoords + 1, outFileName_Farthest) == NULL){
        printf("Error writing Farthest Insertion Tour\n");
    }
    if (writeTourToFile(tourNearest, numOfCoords + 1, outFileName_Nearest) == NULL){
        printf("Error writing Nearest Addition Tour\n");
    }

    // 释放分配的内存
    for(int i = 0; i < numOfCoords; i++){
        free(dMatrix[i]);
    }
    free(dMatrix);
    free(tourCheapest);
    free(tourFarthest);
    free(tourNearest);

    return 0;
}


// int *findCheapestTour(double **dMatrix, int numOfCoords){
//     int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     double minDistance = __DBL_MAX__;
//     TourResult pathResult;
//     pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     pathResult.totalDistance = 0.0;

//     // 遍历起点
//     int i = 0;
//     for (int i = 0; i < numOfCoords; i++)
//     {
// 		// printf("Cheapest Point: %d\n", i);
//         pathResult = cheapestInsertion(dMatrix, numOfCoords, i);
//         if (pathResult.totalDistance < minDistance)
//         {
//             // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
//             minDistance = pathResult.totalDistance;
//             minTour = pathResult.tour;

// 			// TODO	debug
// 			// printf("Cheapest: ");
// 			// for (int i = 0; i < numOfCoords + 1; i++)
// 			// {
// 			// 	printf(" %d", minTour[i]);
// 			// }
// 			// printf("\n");
//         }        
//     }
// 	// free(pathResult.tour);
//     return minTour;
// }


// int *findFarthestTour(double **dMatrix, int numOfCoords){
//     int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     double minDistance = __DBL_MAX__;
//     TourResult pathResult;
//     pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     pathResult.totalDistance = 0.0;

//     // 遍历起点
//     int i = 0;
//     for (int i = 0; i < numOfCoords; i++)
//     {
// 		// printf("Farthest Point: %d\n", i);
//         pathResult = farthestInsertion(dMatrix, numOfCoords, i);
//         if (pathResult.totalDistance < minDistance)
//         {
//             // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
//             minDistance = pathResult.totalDistance;
//             minTour = pathResult.tour;

// 			// TODO	debug
// 			// printf("Farthest: ");
// 			// for (int i = 0; i < numOfCoords + 1; i++)
// 			// {
// 			// 	printf(" %d", minTour[i]);
// 			// }
// 			// printf("\n");
//         }        
//     }
// 	// free(pathResult.tour);
//     return minTour;
// }

// int *findNearestTour(double **dMatrix, int numOfCoords){
//     int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     double minDistance = __DBL_MAX__;
//     TourResult pathResult;
//     pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
//     pathResult.totalDistance = 0.0;

//     // 遍历起点
//     int i = 0;
//     for (int i = 0; i < numOfCoords; i++)
//     {
// 		// printf("Nearest Point: %d\n", i);
//         pathResult = nearestAddition(dMatrix, numOfCoords, i);
//         if (pathResult.totalDistance < minDistance)
//         {
//             // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
//             minDistance = pathResult.totalDistance;
//             minTour = pathResult.tour;

// 			// TODO	debug
// 			// printf("Nearest : ");
// 			// for (int i = 0; i < numOfCoords + 1; i++)
// 			// {
// 			// 	printf(" %d", minTour[i]);
// 			// }
// 			// printf("\n");
//         }        
//     }

	
// 	// free(pathResult.tour);
//     return minTour;
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

