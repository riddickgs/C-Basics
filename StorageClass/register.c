#include <stdio.h>
/* register storage specifier allows user to store data in available registers
 * Storage		register/stack
 * Initial value	garbage
 * Scope		within block
 * Life			end of the block
 * Note: address of register does not exist
 * */
int main() {

  register int a = 1;

  for (;a <10000; a++) {
    
  }
  return 0;
}
