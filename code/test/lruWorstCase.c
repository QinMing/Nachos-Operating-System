/**
 * A program to demonstrate a scenario in which LRU runs worse than other algorithms.
 */

#include "syscall.h"

#define numPhysPages 32
#define size (numPhysPages*128)
int arr[size - 1];

int main() {
  
  int i;
  for (i = 0; i < size - 1; i++) {
    arr[i] = i;
  }
  
  int sum = 0;
  int j;
  for (j = 0; j < 1000000; j += 128) {
    sum += arr[j%(size - 1)];
  }
  
  Exit(sum);
}