#include <stdio.h>

#define SQ(x) (x)*(x)
void callme(const char *a) {
  printf("%c\n", a[10]);
}

int main() {
  int a = 10, s = 7;
  char *arr = "Hello man this is god talking!";
  printf("%10.*s\n", s, arr);
  printf("%d\n", SQ(++s));

  s = ++a + a++ + a--;
  // (++a) + ((a++)+(a--))  || 11+12+12
  //s = ++a + a++;
  printf("s %d\n", s);

  //endianess
  int c = 0x00FF00FF;
  unsigned char *b = (unsigned char *)&c;
  if (*b == 0xFF) printf("Little endian\n");
  else printf("Big endian\n");

  callme(arr);
  printf("%d\n", !2.3); //treats as integer
  printf("%ld\n", sizeof(!2.3));  //here 0 is integer hence will give 4
  return 0;
}
