#include "syscall.h"

int main()
{
	int result;
	Exec("../test/array", 0, 0, 0);
	result = Join(1);//Join on itself

	Exit(result);
}
