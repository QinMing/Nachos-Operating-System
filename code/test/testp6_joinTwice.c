#include "syscall.h"

int main()
{
	int result;

	result = Join(3); //the exit status of process #2 is passed to result

	Exit(result);
}
