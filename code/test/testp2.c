#include "syscall.h"
//int argc;
//char* argv[3];
int main()
{
	int result;
	//argc = 3;
	//argv[0] = "1";
	//argv[1] = "3";
	//argv[2] = "5";
	Exec("../test/array",0,0,0);
	
	Exit(1);
}
