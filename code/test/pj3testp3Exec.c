#include "syscall.h"

int
main()
{
	int result;

	result = Exec("../test/pj3testp3goodLocality", 0, 0, 0);

	Exit(result);
}
