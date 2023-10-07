/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File		int.c
 * Author	Riddick
 * Modify	Riddick|Fri Oct  6 03:07:54 EDT 2023
 * Desc		This file contains integer type input and output
 * Notes	int includes decimal input ranges from 
		signed		-2147483648 to 2147483647
		unsigned	0 to 4294967295
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <limits.h>

/*Mandatory main function | return type 'int'*/
int main() {
  /*Declare integer variable*/
  int a, i;
  unsigned int b;

  printf("Enter a number:\n");

  /*Input the number individually using scanf*/
  scanf("%d", &a); //where %d/%i are signed format specifier
  printf("%d\n", a);

  scanf("%i", &i);
  printf("%i\n", i);

  scanf("%u", &b); //where %u/%o/%x/%X are unsigned format specifier
  printf("%u\n", b);

  scanf("%o", &b);
  printf("%o\n", b); //unsigned octal representation

  scanf("%x", &b);
  printf("%x\n", b); //unsigned hexadecimal representation; abcdef

  scanf("%X", &b);
  printf("%X\n", b); //unsigned hexdecimal representation; ABCDEF

  /*Input the number together*/
  scanf("%d %i", &a, &i);

  /*print input data together*/
  printf("%d %i\n", a, i);

  printf("Size of integer:\n\t[a] %ld [i] %ld [b] %ld\n",
          sizeof(a), sizeof(i), sizeof(b));

  printf("Address %p\n", &a);
  /*Range of int data type*/
  printf("Range of int \nsigned\t\t[%d to %d]\nunsigned\t[0 to %u]\n",
	 INT_MIN, INT_MAX, UINT_MAX);
  
  return 0;
}
