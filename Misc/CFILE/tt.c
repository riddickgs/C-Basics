#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define GET(a) Get(a)

int Get(int a) {
return a;
}
int main()
{
  char *ptr = GET(FALSE)?"TRUE":"FALSE";
  printf("%p\n", ptr);
  return 0;
}
