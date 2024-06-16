//Program to count set bits with multiple ways

#include <stdio.h>

unsigned char countSetBits(unsigned int v) {
  unsigned int p;
#ifdef LOOKUP //Using lookup table
  static const int lookuptable256[] = {
#define B2(n) n, n+1, n+1, n+2
#define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
	B6(0), B6(1), B6(1), B6(2)
  };
  p =	lookuptable256[v & 0xff] +
	lookuptable256[(v>>8) & 0xff] +
	lookuptable256[(v>>16) & 0xff] +
	lookuptable256[v>>24];
#elif BUILTIN //using built_in method
  p = __builtin_popcount(v);
#else //Brian Kernighan's Method
  for (p = 0; v; p++)
	 v &= v-1; //clear the least significant bit set
#endif
  return p;
}

int main() {
  printf("%d\n", countSetBits(0xFF1177AD));
  return 0;
}
