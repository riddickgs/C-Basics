/* blocksizeof()*/

#include <stdio.h>
#include <stdlib.h>
#define blocksizeof(a) ((sizeof(a)+1)&~1)

typedef struct {
  unsigned a;
  unsigned b;
  unsigned c;
  unsigned long d;
  void *p;
  char *i;
  char s[8];
} XY;

int main() {

  //printf("%ld\n", blocksizeof(XY));
  //printf("%x\n", ~1U);
  char *str = "hello there!!";
  return 0;

}
