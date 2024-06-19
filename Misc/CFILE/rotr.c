#include <stdio.h>
//Thus function rotates right
#define ROTR(x, n) ((x>>n) |(x<<(32-n)))
int main() {
   unsigned int a = 0xAA00EE00;
   printf("Before %X\tAfter %X\n", a, ROTR(a, 8));
   printf("Before %X\tAfter %X\n", a, ROTR(a, 16));
  return 0;
}
