/* Program to see how memory is shared with multiple variable variables inside union
 * Output:
4
Members:        a:1 b:1:c:1

Note: Union can take only one value at a time so here it initializes only once i.e. with (1)
      size of union is maximum size of datatype in union i.e. size of int (4)
*/
#include <stdio.h>

union myunion {
  int a;
  short b;
  char c;
};

int main() {
/* union1.c: In function ‘main’:
union1.c:11:26: warning: excess elements in union initializer
   11 |   union myunion un = {1, 2, 3};
      |                          ^
union1.c:11:26: note: (near initialization for ‘un’)
union1.c:11:29: warning: excess elements in union initializer
   11 |   union myunion un = {1, 2, 3};
      |                             ^
union1.c:11:29: note: (near initialization for ‘un’)
 * */
  union myunion un = {1, 2, 3};
  printf("%lu\nMembers:\ta:%d b:%d:c:%d\n", sizeof(un), un.a, un.b, un.c);
  return 0;
}
