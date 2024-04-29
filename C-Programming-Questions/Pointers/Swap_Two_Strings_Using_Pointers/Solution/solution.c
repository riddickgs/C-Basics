/*  Implement a program that swaps two strings using pointers to pointers.
                                                                                                                                                WTD: Use a pointer to pointer to swap the base addresses of the two strings.                                                                                                                                                                                                                    (e.g.: I/P: char *str1 = "hello", *str2 = "world" ,O/P: str1 = "world", str2 = "hello" ) */

#include <stdio.h>

void swapString(char *s1, char *s2) {
  while (*s1 != '\0' && *s2 != '\0') {
    *s1 ^= *s2; // 2 ^ 3 = 10 ^ 11 = 01 
    *s2 = *s1 ^ *s2; // 01 ^ 11 = 10
    *s1 = *s1 ^ *s2; // 01 ^ 10 = 11
    s1++;s2++;
  }
}

int main() {
  char s1[] = "hello", s2[] = "world";

  swapString(s1, s2);
  printf("%s %s\n", s1, s2);
  return 0;
}
