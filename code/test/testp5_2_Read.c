/* Test file that triggers an AddressErrorException */

#include "copyright.h"
#include "syscall.h"

int main() {

	Write(" $", 2, ConsoleOutput);

	int *i;
	int ar[5];
	i = ar[5];
	int res = Read(*i, 1, ConsoleInput);
	Exit(res);
}
