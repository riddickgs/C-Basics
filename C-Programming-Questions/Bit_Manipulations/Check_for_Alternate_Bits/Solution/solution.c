/* Check for Alternate Bits. Write a function to check if bits in a given number are in alternate pattern.
                                                                                                                                 WTD: Given an integer, check if the bits in its binary representation alternate between 0 and 1. Use bitwise operations to traverse the bits and perform the check.                                                                                                                                                                                                                                (e.g.: I/P:  0b10101010; O/P: True)
 Algo:
 get last bit = x & 1U
 get second last bit = (x >> 1) & 1U
 compare till number exist x >>= 1
 * */

#include <stdio.h>
#define T_BITS 32
int isAltPattern(unsigned int x) {
  if(!x) return 0;

  while (x > 0) {
    //last bit and second last bit
    if ((x & 1U) == ((x>>1) & 1U)) return 0;
    x >>= 1;
  }
  return 1;
}

int main() {
  unsigned int x = 0;
  printf("%s\n", (isAltPattern(x))?"True":"False");
  return 0;
}
