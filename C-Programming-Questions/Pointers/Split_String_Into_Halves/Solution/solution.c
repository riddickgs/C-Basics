/* Implement a function that splits a string into two halves and returns pointers to the beginning of each half.
                                                                                                                                                WTD: Use pointer arithmetic to find the middle of the string. Return the original and the middle pointers.                                                                                                                                                                                      (e.g.: I/P: "HelloWorld" ,O/P: "Hello", "World" ) */

#include <stdio.h>

char *stringHalf(char *s, char *e)
{
  char *mid;
  while (s < e)
  {
    s++;e--;
  }
  return s;
}

int main() {
  char s[] = "HelloWorld";

  printf("%s %s\n", s, stringHalf(s, s+sizeof s/sizeof s[0]-2));
  return 0;
}
