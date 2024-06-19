// Question: can register variable be global
#include <stdio.h>

register int aa = 15;

int main() {
	register int b = 12;
	printf("%d %d", b, aa);
  return 0;
}
