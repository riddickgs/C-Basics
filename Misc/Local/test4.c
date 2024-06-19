#include <stdio.h>

int main() {
  char c;
  int d;

  while (scanf("%d%c", &d, &c) && c != '\n') {
    printf("%d", d);
  }
  printf("Exited loop\n");
  return 0;
}
