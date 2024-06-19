#include <stdio.h>

static int add(int a, int b) {
  static int c;
  printf("inside add %d\n", c);
  c  = a+b;
  return c;
}
int main() {
  int  i = add(10,20);
  printf("i = %d\n", i);
  int c = add(11, 22);
  printf("c = %d\n", c);
  return 0;
}
