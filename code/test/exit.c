/* halt.c
 *	Simple program to test whether running a user program works.
 *
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"
#define NUM 40
int
main()
{
	int a[NUM];
	int sum = 0;
	int i;
	int j;
	for (i=0;i<NUM;i++){
		a[i]=i;
	}
	for (j=0;j<NUM;j++){
		sum+=a[j];
	}
	//a[0]=1;
	//a[1]=2;
	//a[2]=3;
	Exit(sum);
}
