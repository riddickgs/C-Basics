#include <stdio.h>

int main() {
  unsigned int n = 0x00343434;
  printf("Before nibble swapping: %X\n", n);
  n = ((n >> 4) & 0x0f0f0f0f)|((n << 4) & 0xf0f0f0f0);
  printf("After nibble swapping: %X\n", n);
  return 0;
}
