#include <stdio.h>
#include <string.h>

int main() {
  char inp[3000];
  int w;
  scanf("%d\n%[^\n]s", &w, inp);
  printf("%d %s\n", w, inp);
  return 0;

}
