#include <stdio.h>
#include <stdlib.h>

int main() {
  unsigned int i = 0x11223344;

  printf("%X\n", i);
  //swapping 2 bytes
  printf("%X\n", ((i<<16)|(i >> 16)));
  return 0;
}
