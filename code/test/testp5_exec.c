/* Test file that triggers an IllegalInstrException in an Exec'd process */

#include "syscall.h"

int main() {

  // Exec process that triggers exception
  Exec("../test/testp5",0,0,0);


  // Exec'd process should have terminated from exception,
  // but this process should still reach this point and exit normally
  Exit(0);
}
