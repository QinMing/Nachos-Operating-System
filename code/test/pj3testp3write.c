#include "syscall.h"
#define NumPhysPages    32

int array[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49 };

int arrayA[50][50];

main()
{
	int sum;
	int i;
	int j;

	sum = 0;


	for (i = 0; i <50; i++){
		for (j = 0; j < 50; j++){
			arrayA[i][j] = array[j];
		}
	}

	for (i = 0; i <50; i++){		//rewrite the arrayA to show correct handling of dirty pages
		for (j = 0; j < 50; j++){
			arrayA[i][j]++;
		}
	}

	for (i = 0; i <50; i++){		//rewrite the arrayA to show correct handling of dirty pages
		for (j = 0; j < 50; j++){
			arrayA[i][j]++;
		}
	}

	for (i = 0; i < 50; i++) {		//adding the arrayA up the sum should be 1225*50+2500+2500=66250
		for (j = 0; j < 50; j++){
			sum += arrayA[i][j];
		}
	}

	Exit(sum);
}
