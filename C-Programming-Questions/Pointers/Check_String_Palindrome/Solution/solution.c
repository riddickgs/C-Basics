/*Create a program that checks if a string is a palindrome using pointers.
                                                                                                                                 WTD: Use two pointers, one at the start and the other at the end. Traverse inward, comparing characters.                                                                                                                                                          (e.g.: I/P: "radar" ,O/P: True) */

#include <stdio.h>
#include <string.h>
int isPalindrome(char *s, char *e) {
  while (s <= e) {
    if (*s != *e) return 0;
    s++;e--;
  }
  return 1;
}

int main() {
  char arr[] = "ror"; //start = arr, end = arr + strlen(arr) - 1 || arr + 
  printf("%s\n", (isPalindrome(arr, arr+sizeof arr/sizeof arr[0] - 2))?"True":"False");
  return 0;
}
