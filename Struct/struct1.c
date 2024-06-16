/* Program to show how structure is declared and find its size
 * Output
sizeof structure 8
 * Ways to Initialize
 * */

#include <stdio.h>

struct ms {
  int a;
  short b;
  char c;
};

int main() {
  struct ms s = {1, 2, 'c'}; //Auto order initialization
  struct ms x = {.a = 2, .b = 7, .c = 'N',}; //inorder initialization
  printf("sizeof structure %lu\n", sizeof(s));
  printf("Initial Members s\ta:%d b:%d c:%c\n", s.a, s.b, s.c);
  printf("Initial Members x\ta:%d b:%d c:%c\n", x.a, x.b, x.c);
  s.a = 1234;
  s.b = 22;
  s.c = 'A';
  printf("Updated Members\ta:%d b:%d c:%c\n", s.a, s.b, s.c);
  return 0;
}
