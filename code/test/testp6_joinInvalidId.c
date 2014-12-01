#include "syscall.h"

int main()
{
	int result;

	result = Join(-1);

	Exit(result);
}
