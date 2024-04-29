/* Swap Odd and Even Bits. Implement a function to swap odd and even bits in an integer.

WTD: For a given integer, swap its odd and even bits. Bit 1 should swap with Bit 2, Bit 3 with Bit 4, and so on. Use bitwise operations to perform the swapping.

(e.g.: I/P:  0b10101010; O/P: 0b01010101)

 CFLAGS=-Wall -Werror
 gcc $(CFLAGS) xyz.c -o xyz -DSWAP
 gcc $(CFLAGS) xyz.c -o xyz
 E.T (ticks) 			swap	 without swap
 0				39	 3
 1				1	 2
 2				1	 1
 3				0	 0
 *********************************************************/

#include <stdio.h>
#include <time.h>

typedef unsigned int UI;
//syntax: swapbits(number, position1, position2, number of bits);
UI swapbits(UI x, UI pos1, UI pos2, UI n) {
  UI xor = ((x >> pos1)^(x >>pos2)) & ((1U<<n)-1);
  return x ^ ((xor << pos1) | (xor <<pos2));
}

void swapEOBits(UI *x) {
#ifdef SWAP
  *x = swapbits(*x, 0, 1, 2); //swap even and odd
  *x = swapbits(*x, 2, 3, 2);
  *x = swapbits(*x, 4, 5, 2);
  *x = swapbits(*x, 6, 7, 2);
#else
  unsigned int oddBits, evenBits;
  evenBits  = *x & 0xAAAAAAAA;
  oddBits   = *x & 0x55555555;

  evenBits >>= 1U;
  oddBits <<=1U;
  *x = evenBits|oddBits;
#endif
}

int main() {
  clock_t ticks = clock();
  unsigned int x = 0b10101010;
  swapEOBits(&x);
  //checking number ticks taken to solve using clock() from 2 methods
  printf("%hhx\nticks %ld\n", x, clock() - ticks);
  return 0;
}
