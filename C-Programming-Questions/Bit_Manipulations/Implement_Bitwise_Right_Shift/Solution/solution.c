#include <stdio.h>

int rightshift(unsigned int a, int b) {
  return (a /= (1<<b));
}
int main() {
  unsigned int a = 0b11010011;
  printf("%d", rightshift(a, 4));
  return 0;
}
