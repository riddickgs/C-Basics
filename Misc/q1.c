//when char pointer is pointing to an interger which byte it pick
#include <stdio.h>

int main() {
  int a = 320; // 00000001 01000000
  char *ptr = (char *)&a;
  printf("%d\n", ptr);
  return 0;
}
