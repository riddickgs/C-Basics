/* Program to show use on union in a practical program
 *
 * Output:
4
Members Date 100607e8
Binary: 00010000 00000110 00000111 11101000
           day     month        Year
	   16	    6     1024+512+256+128+64+32+8 = 2024
 * */

#include <stdio.h>

union myun {
  int date;
  struct {
    int yy:16;
    int mm:8;
    int dd:8;
  };
};

int main() {
  //stoarage only took 4 bytes instead on 12 bytes for day, month and year
  union myun mn = {
  .yy = 2024,
  .mm = 6,
  .dd = 16,
  };
  printf("%lu\nMembers\tDate %x\n", sizeof(union myun), mn.date);
  //Union members should be printed individually
  printf("Year:%d\t", mn.yy);
  printf("Month:%d\t", mn.mm);
  printf("Day:%d\n", mn.dd);
  return 0;
}
