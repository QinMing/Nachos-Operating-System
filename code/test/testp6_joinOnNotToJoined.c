#include "syscall.h"

int main()
{
	int result;

	Exec("../test/array", 0, 0, 0);
	result = Join(2);//process #2's willJoin is 0

	Exit(result);
}
