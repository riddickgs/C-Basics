#include <stdio.h>

/* static storage specifier makes visibility within file where declared
 * Storage		data
 * Initial Value	0
 * Scope		within block
 * Life			end of the program
 * */

static void callme(int arg) {
  printf("static function\n");
}
int main() {
  static int a;

  printf("static variable %d\n", a);
  callme('5');
  return 0;
}
