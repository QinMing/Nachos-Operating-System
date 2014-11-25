/* Test file that trigger an IllegalInstrException  */

#include "copyright.h"
#include "syscall.h"


int main() {

  int a = 20;
  int x = a / 0;

  Exit(0);

}
