#include <stdio.h>
#include <stdlib.h>

int f = 29;

int main() {
  int f = 0;
  {
    int i = 10;
    extern int f;
    printf("Inside Block|Local i is:\t%d\nInside Block|Global f is:\t%d\n", i, f);
  }
  call();
  return 0;
}
