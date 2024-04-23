#include <stdio.h>

/* Default storage specifier of datatyoe is automatic 
 * Storage 		stack
 * Initial value 	garbage
 * Scope 		within block
 * Life  		end of the block
 */
int main() {
  auto int i = 5;
  int j;
  printf("auto variable %d garbage %d\n", i, j);
  return 0;
}
