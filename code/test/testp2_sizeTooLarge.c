#include "syscall.h"

int
main()
{
	int result;

	result = Exec("../test/largeSize", 0, 0, 0);

	Exit(result);
}
