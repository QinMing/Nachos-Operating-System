#include "syscall.h"


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
	for (i = 0; i < 50; i++) {
		sum += array[i];
	}

	for (i = 0; i <10; i++){
		for (j = 0; j < 50; j++){
			arrayA[i][j] = array[j];
		}
	}

	Exit(sum);
}
