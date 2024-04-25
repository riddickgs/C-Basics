#include <stdio.h>

int power(int a, int b) {
  if (!b) return 1;
  if (!a) return 0;

  //product of 'a' 'b' times
  //0b11 ^ 0b100  = 0b1010001
  printf("%d", 1 <<2);

}

int main() {
  printf("%d", power(a, b));
  return 0;
}
