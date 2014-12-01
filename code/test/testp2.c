#include "syscall.h"

int main()
{
	int result;
	
	result = Exec("../test/array",0,0,0);
	
	Exit(result);
}
