#include <stdio.h>
/* extern storage specifier make global variable accessible to file
 * Storage 		data segment
 * Initial value 	0
 * Scope 		global multiple files
 * Life 		end of the program
 * */
int f = 29;

int main() {
  int f = 0;
  {
    int i = 10;
    extern int f;
    printf("Inside Block|Local i is:\t%d\nInside Block|Global f is:\t%d\n", i, f);
  }
  printf("Local f is: %d\n", f);
  return 0;
}
