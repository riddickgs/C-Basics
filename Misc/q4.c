//Calculate the size of structure and see what is size due to padding done by compiler
#include <stdio.h>
#include <stdlib.h>

struct new {
  char a;
  double b;
  int c;
};

int main() {
  static struct new s;

  printf("size %lu\n", sizeof(s));  
  return 0;
}
