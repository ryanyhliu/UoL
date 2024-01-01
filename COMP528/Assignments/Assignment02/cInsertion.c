#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<omp.h>
#include<string.h>

typedef struct {
    int *tour;
    double totalDistance;
} TourResult;

int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void *writeTourToFile(int *tour, int tourLength, char *filename);
double **createDistanceMatrix(double **coords, int numOfCoords);
double sqrt(double arg);
// int *cheapestInsertion(double **dMatrix, int numOfCoords);
TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd);
int *findTour(double **dMatrix, int numOfCoords);

int main(int argc, char *argv[]){
	char filename[500];
	char outFileName[500];

	strcpy(filename, argv[1]);
	strcpy(outFileName, argv[2]);

	if(filename == NULL || outFileName == NULL){
		printf("Error reading files");
		return 1;
	}

	int numOfCoords = readNumOfCoords(filename);

	double **coords = readCoords(filename, numOfCoords);

	double tStart = omp_get_wtime();

	double **dMatrix = createDistanceMatrix(coords, numOfCoords);
	// int *tour = cheapestInsertion(dMatrix, numOfCoords);
	int *tour = findTour(dMatrix, numOfCoords);

	
	double tEnd = omp_get_wtime();

	printf("\nTook %f milliseconds", (tEnd - tStart) * 1000);

	if (writeTourToFile(tour, numOfCoords + 1, outFileName) == NULL){
		printf("Error");
	}

	for(int i = 0; i < numOfCoords; i++){
		free(dMatrix[i]);
	}

	free(dMatrix);
	free(tour);

}

int *findTour(double **dMatrix, int numOfCoords){
    int *minTour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    double minDistance = __DBL_MAX__;
    TourResult pathResult;
    pathResult.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    pathResult.totalDistance = 0.0;

    // 遍历起点
    int i = 0;
    for (int i = 0; i < numOfCoords; i++)
    {
        pathResult = cheapestInsertion(dMatrix, numOfCoords, i);
        if (pathResult.totalDistance < minDistance)
        {
            // printf("---TEST 05: point: %d; pathDis: %f \n", i, pathResult.totalDistance);
            minDistance = pathResult.totalDistance;
            minTour = pathResult.tour;
        }        
    }
	// free(pathResult.tour);
    return minTour;
}

TourResult cheapestInsertion(double **dMatrix, int numOfCoords, int pointOfStartEnd){
	TourResult result;
    result.tour = (int *)malloc((numOfCoords + 1) * sizeof(int)); 
    result.totalDistance = 0.0;
	
	//Setting up variables
	int i, j;
	int nextNode, insertPos;

	//Memory allocation for the tour and visited arrays. Tour is numOfCoords + 1 for returning to origin
	int *tour = (int *)malloc((1 + numOfCoords) * sizeof(int));
	bool *visited = (bool *)malloc(numOfCoords * sizeof(bool));

	//Initialising tour to empty
	for(i = 0; i < numOfCoords; i++){
		tour[i] = -1;
	}

	//Tour always starts with 0. 0 is visited
	tour[0] = pointOfStartEnd;
	tour[1] = pointOfStartEnd;
	visited[pointOfStartEnd] = true;
	
	//Hard coding because I'm lazy
	int numVisited = 1;
	int tourLength = 2;
	
	//While there are still unvisited vertices
	while(numVisited < numOfCoords){
		double minCost = __DBL_MAX__;
		double thisDistance = 0.0;

		//Find an unvisited vertex such that the cost of adding it to the tour is minimal 
		for(i = 0; i < tourLength - 1; i++){
			for(j = 0; j < numOfCoords; j++){
				if(!visited[j]){
					double cost = dMatrix[tour[i]][j] + dMatrix[j][tour[i + 1]] - dMatrix[tour[i]][tour[i + 1]];
					if(cost < minCost){
						minCost = cost;
						nextNode = j;
						insertPos = i + 1;
						thisDistance = cost; // 增加的路径长度
					}
				}
			}
		}		

		//Shift array to add next vertex
		for(i = numOfCoords; i > insertPos; i--){
			tour[i] = tour[i - 1];
		}

		tour[insertPos] = nextNode;
		visited[nextNode] = true;
		
		tourLength++;
		numVisited++;
		result.totalDistance += thisDistance;
	}

	result.tour = tour;

	// free(tour);
	free(visited);

	return result;
}

double **createDistanceMatrix(double **coords, int numOfCoords){
	int i, j;
	
	double **dMatrix = (double **)malloc(numOfCoords * sizeof(double *));

	for(i = 0; i < numOfCoords; i++){
		dMatrix[i] = (double *)malloc(numOfCoords * sizeof(double));
	}

	for(i = 0; i < numOfCoords; i++){
		for(j = 0; j < numOfCoords; j++){
			double diffX = coords[i][0] - coords[j][0];
			double diffY = coords[i][1] - coords[j][1];
			dMatrix[i][j] = sqrt((diffX * diffX) + (diffY * diffY));
		}
	}

	return dMatrix;

}


