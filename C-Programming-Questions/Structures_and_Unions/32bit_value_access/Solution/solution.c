#include <stdio.h>
typedef union un {           //According to little endian LSB will
  unsigned int four;         // [0] 34 [1] 12  [2] AB [3] CD
  unsigned short two[2];     // [0] 3412       [1] ABCD
  unsigned char one[4];      // [0] 34 [1] 12  [2] AB [3] CD
} UN32;

typedef union un1 {
  unsigned int four;
  struct {
    unsigned short x1;
    unsigned short x0;
  };
  struct { //little endian byte ordering
    unsigned char c3;
    unsigned char c2;
    unsigned char c1;
    unsigned char c0;
  };
} UN132;

int main() {
  UN32 a, b, c;
  printf("Enter a 4 byte value in hex\n");
  //scanf("%X", &a.four);
  a.four = 0XABCD1234;
  c = a;
  b = a;
  printf("16-bits: %#X, %#X; 8-bits: %#X, %#X, %#X, %#X\n", b.two[1], b.two[0], c.one[3], c.one[2], c.one[1], c.one[0]);

  UN132 a1, a2, a3;
  a1.four = 0xABCD1234;
  a3 = a2 = a1;
  printf("16-bits: %#X, %#X; 8-bits: %#X, %#X, %#X, %#X\n", a2.x0, a2.x1, a3.c0, a3.c1, a3.c2, a3.c3);

  return 0;
}
