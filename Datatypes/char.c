/* File 	char.c
   Purpose	character input
   Author	Riddick
   Modify	Riddick@Thu Oct  5 14:59:00 EDT 2023
*/
#include <stdio.h>

/* Mandatory function main with return type 'int'*/
int main() {
  /*Take a character variable*/
  char a;

  printf("Enter a character:\n");

  /*Input the character data*/
  scanf("%c", &a);

  /*Print the character data*/
  printf("%c\n", a);

  /*size of character variable*/
  printf("Size %ld\n", sizeof(a));

  /*Address of the character variable*/
  printf("Address %p\n", &a);

  /*Return type int so returned 0 on success*/
  return 0;
}
