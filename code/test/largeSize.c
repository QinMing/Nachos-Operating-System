/*
* array.c
*
* A simple program that has code and data and fits into memory.
*/
#include "syscall.h"

//int Exec(char *, int, char **, int);
//%int Exit(int);

int array[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49 };

int arrayA[70][50];

void
verify_array(int a[],int n)
{
	int i;
	
	for (i = 0; i < n; i++) {
		if (a[i] != i) {
			Exit(-1);
		}
	}
}

int
add_array(int a[],int n)
{
	int i, sum;

	sum = 0;
	for (i = 0; i < n; i++) {
		sum += a[i];
	}
	return sum;
}

int
main()
{
	int sum;
	int i;
	int j;

	verify_array(array,48);
	sum = add_array(array,48);
	verify_array(array,48);
	for (i = 0; i <50; i++){
		for (j = 0; j < 50; j++){
			arrayA[i][j] = array[j];
		}
	}
	Exit(sum);
}
