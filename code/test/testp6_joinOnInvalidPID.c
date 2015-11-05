#include "syscall.h"

int main()
{
	int result;

	Exec("../test/array", 0, 0, 1);
	result = Join(5);//there is no process #5

	Exit(result);
}
