#include "syscall.h"
int main()
{
	int result;
	char str[13] ="../test/array";
	result=Exec(str,0,0,0);
	Exit(result);
}
