#include <stdio.h>
/* Algo:
 * if last bit is set then multiply number by result
 * Save number multiply by itself in number
 * do right shift of exponent on every iterations*/
int power(int a, int b) {

  long ans = 1, lastbit;
  //product of 'a' 'b' times
  //0b11 ^ 0b100  = 0b1010001
  //0b11 ^ 0b11 = 0b11011
  while (b > 0) {
    lastbit = (b & 1);
    if (lastbit) ans = ans * a;
    a = a * a;
    b >>= 1;
  }
  return ans;
}

int main() {
  int a  = 3, b = 3;
  printf("%d\n", power(a, b));
  return 0;
}
