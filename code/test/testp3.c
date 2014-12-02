/*
to test part 3 Exec() syscall. testp3_addition returns the sum of argments
*/



#include "syscall.h"
int argc;
char* argv[3];
int main()
{
    int result = 1000;
	argc = 3;
	argv[0]="1";
	argv[1]="3";
	argv[2]="5";
    Exec("../test/testp3_addition", argc, argv, 0);
    Exit(0);
}