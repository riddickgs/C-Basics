/*Write a function that trims leading and trailing whitespace from a string using pointers.
                                                                                                                                                WTD: Use two pointers to find the first and last non-whitespace characters. Move characters to trim the string                                                                                                                                                                                  (e.g.: I/P: " Hello World " ,O/P: "Hello World" )*/
// do left trim first and then right trim of spaces
#include <stdio.h>
#include <ctype.h>
#include <string.h>
char *ltrim (char *s) {
  while (*s != '\0' && isspace(*s))
    s++;
  return s;
}

char *rtrim(char *s) {
  char *e = s+strlen(s)-1; 
  while (e > s && isspace(*e))
	  e--;
  *(e+1) = '\0'; //last alphabet is found
  return s;
}

//sanity check done before
char *trim_whitespace(char *s) {
  if (!s) return 0;
  if (!*s) return 0;
  s = ltrim(s);
  s = rtrim(s);

  return s;
}

int main() {
  char arr[] = " Hello World ";
  printf("$$%s$$\n", trim_whitespace(arr));
  return 0;
}
