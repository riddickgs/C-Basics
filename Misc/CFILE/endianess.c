#include <stdio.h>

int main() {
  int i = 150; //0x00000000
	       //0x00000000 = 0x10010110 = 0x11101001 = 64+32+8+2 = -106
  char *en = (char *)&i;
  //Little endian if LSB at Lower address // big endian LSB at Higher address
  //Little endian if MSB at Higher address // big endian MSB at Lower address
  //Big endian 0   1 Little endian 1   0
  //         MSB   LSB           LSB   MSB 
  printf("%d\n", (char)*en);
  if (*en == -106) { // sig
    printf("Little Endian\n");
  } else {
    printf("Big Endian\n");
  }

  return 0;
}
