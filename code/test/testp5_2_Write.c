/* Test file that triggers an AddressErrorException */

#include "copyright.h"
#include "syscall.h"

int main() {

	int *i;

	int ar[5];
	i = ar[5];
	int res = Write(*i, 1, ConsoleOutput);
	Exit(res);
}
