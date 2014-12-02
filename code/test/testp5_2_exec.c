/* Test file that triggers an AddressErrorException in an Exec'd process */

#include "syscall.h"

int main() {
	// Exec process that triggers exception

	Exec("../test/testp5_2_Normal", 0, 0, 0);
	Exec("../test/testp5_2_Write", 0, 0, 0);
	Yield();
	int pid = Exec("../test/testp5_2_Read", 0, 0, 1);

	// Exec'd process should have terminated,
	// but this process should still reach this point and exit normally
	Join(pid);
	Yield();
	Exit(0);
}
