#include <stdio.h>
#include <string.h>

int main() {
  char *ptr1 = NULL;
  char *ptr2 = 0;

  //segmentation fault
  strcpy(ptr1, "c");
  strcpy(ptr2, "questions");
  printf("%s %s\n", ptr1, ptr2);
  return 0;
}
