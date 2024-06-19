/*Problem: Print the first non-repeated character from a string.
WTD: Examine the string and pinpoint the very first character that doesn't repeat elsewhere in the string.
e.g.: I/P: "swiss", O/P: "w"

O(n*n)
 */

#include <stdio.h>
#include <string.h>

int count(char *s, char c) {
  int n = 0, i = 0;
  while(s[i] != '\0') {
    if (s[i] == c) n++;
    i++;
  }
  return n;
} 
void printUnq(char *buf) {
  char *ptr = buf;

  while (*ptr != '\0') {
    if (count(buf, *ptr) == 1) break;
    ptr++;
  }
  printf("%c\n", *ptr);
}

int main() {
  char buf[128];

  if (fgets(buf, 128, stdin) == NULL) {
    perror("fgets");
    return -1;
  }
  if (!buf[0]) return -1;
  printUnq(buf);
  return 0;
}
