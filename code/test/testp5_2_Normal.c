/* Test file that triggers an AddressErrorException */

#include "copyright.h"
#include "syscall.h"

int main() {

	int *i;
	i = 65533;
	( *i ) = 2;

	Exit(0);
}
