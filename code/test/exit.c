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
char* argv[3];
char* st;
int main()
{
	char str[300];
	int i;
	for (i=0;i<300;i++)
		str[i]='y';
	argv[0]="ABC";
	argv[1]="DEF1234";

	argv[2]="GHIJKL";
	int result = Exec("../test/exit6", 3, argv, 0);
	Exit(result);
}
