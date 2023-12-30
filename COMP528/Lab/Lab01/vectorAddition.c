#include<stdio.h>
#include<stdlib.h>
#include<time.h>


int main(void){
	
	const int ARRSIZE = 10; 

	int arr1[ARRSIZE];

	int i;

	srand(time(NULL)); //Initialise random seed. Seed will be based on time.

	for(i = 0; i < ARRSIZE; i++){
		arr1[i] = rand() % 100; //assign arr1 with random values between 0-99

		//TASK: Initialise arr2 with random variables
		
	}

	//Print Array1 Values

	printf("Array1: ");
	printArray(arr1, ARRSIZE);

	//TASK: Print Array2 Values

	/*
	TASK: Declare a new array, and sum up element by element arr3[i] = arr1[i] + arr2[i]
	Print arr3 after you are done
	*/

	///////////////////////////////////////////////////////////////////////
	// Liu Yongheng's Code is here
	
	// Declare a new array
	int arr2[ARRSIZE];
	int arr3[ARRSIZE];

	// Init arr2
	srand(time(NULL));
	
	for (i = 0; i < ARRSIZE; i++){
		arr2[i] = rand () % 100; // Between 0 & 99
	}

	// Output arr2, check
	printf("\nArray2: ");
	printArray(arr2, ARRSIZE);
	
	// Plus 2 arrays
	for (i = 0; i < ARRSIZE; i++){
		arr3[i] = plus(arr1[i], arr2[i]);	
	}

	
	// Output arr3, check
	printf("\nArray3: ");
	printArray(arr3, ARRSIZE);

}

// plus 2 nums
// return sum
int plus(int num1, int num2){
	if (num1 + num2 < 0){
		printf("\n ALERT, Something wrong");
		printf("\n Current Num1 & Num2 is %d and %d", &num1, &num2);	
	}else{
		return num1 + num2;
	}
}

//Function for printing the array
int printArray(int array[], int ARRSIZE){
	int i;
	for(i = 0; i < ARRSIZE; i++){
		printf("%d ", array[i]);
	}
} 

