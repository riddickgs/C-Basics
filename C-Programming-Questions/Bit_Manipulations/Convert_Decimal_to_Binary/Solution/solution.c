#include <stdio.h>
#include <string.h>
//itoa
void decToBin(unsigned int a, char *buf) {
  char i;
  buf[0] = '0';
  buf[1] = 'b';
  char *ptr = buf+2;

  while (a) {
    *ptr = 48+(a & 1);
    a >>= 1;
    ptr++;
  }
  *ptr = '\0';
}

int main() {
  char buf[35];
  unsigned int a = 0xFF;
  decToBin(a, buf);
  printf("%s\n", buf);
  return 0;
}
