#include "syscall.h"

int main()
{
	int result;
	
	Exec("../test/array", 0, 0, 1);
	result=Join(2); //the exit status of process #2 is passed to result

	Exit(result);
}
