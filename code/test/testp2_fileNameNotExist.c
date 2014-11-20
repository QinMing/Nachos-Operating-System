#include "syscall.h"
int main()
{
	int result;
	result=Exec("../test/aarray",0,0,0);//a filename that does not exist, return Spaceid 0
	Exit(result);
}
