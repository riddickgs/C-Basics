#include <stdio.h>

int main() {
  int a[] = {1,2, 3, 4, 5, 6,7,8,9,10};
  int i = 5;
  a[i] = i++;
  printf("i %d a[] %d\n", i, a[i]);
  return 0;
}
