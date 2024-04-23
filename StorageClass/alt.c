#include <stdio.h>
extern int f;

void call(void) {
  printf("Global f %d\n", f);
}
