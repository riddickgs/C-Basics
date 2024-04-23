#include <stdio.h>
#define ISPOWEROF_2(x) ((x) & (x-1))
int main() {
  int n = 1;
  if (ISPOWEROF_2(n) == 0) printf("True");
  else printf("False");
  return 0;
}
