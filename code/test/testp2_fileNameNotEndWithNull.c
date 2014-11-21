#include "syscall.h"
int main()
{
	int result;
	int i;
  char str[13] ="../test/array111111111";
 
	result=Exec(str,0,0,0);
	Exit(result);
}
