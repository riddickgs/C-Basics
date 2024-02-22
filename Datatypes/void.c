#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//void types are used in function and pointers
//
//void in function where there is no return type
void f1()
{
  printf("This is a void function\n");
}

//void as a argument which means no arguments allowed
int f2(void)
{
  printf("void as a argument\n");
}

//void as a pointer return type as well as argument
void *f3(void *arg) 
{
   if (!arg) return NULL;

   int size = *(int *)arg;
   void *addr = malloc(size);
   return addr;

}
int main() {

  f1();
  f2();
  int a = 10;
  char *b = "Riddick!";
  void *c = f3(&a);
  memset(c, 0, a);
  memcpy(c, b, a);
  printf("%s\n",(char*)c);
  free(c);

  return 0;
}
