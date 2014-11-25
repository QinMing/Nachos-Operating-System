/* Test file that triggers an AddressErrorException in an Exec'd process */

#include "syscall.h"

int main() {
  // Exec process that triggers exception
  Exec("../test/testp5_2",0,0,0);

  // Exec'd process should have terminated,
  // but this process should still reach this point and exit normally
  Exit(0);
}
