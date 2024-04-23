#include <stdio.h>
/* input  0000 0101  5
 * output 0001 0001  17
 * usign xor we can toggle the bits 
 * 1 ^ X = !X 
 * */
void toggle_3rd_and_5th_bit(unsigned char *v) {
  unsigned char i = *v;
  *v ^= (1 << 5)|(1 << 3);
}

int main() {
  unsigned char a = 0b10000101;
  printf("%X\n", a);
  toggle_3rd_and_5th_bit(&a);
  printf("%X\n", a);
  return 0;
}
