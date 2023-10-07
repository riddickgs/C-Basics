/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File		float.c
 * Author	Riddick
 * Modify	Riddick|Sat Oct  7 12:58:24 EDT 2023
 * Desc		This file contains float type input and output
 * Notes	float type includes input ranges from 
		positive	1.175494e-38 to 3.402823e+38
		negative	-1.175494e-38 to -3.402823e+38
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <float.h>

/*Mandatory main function |return type 'int'*/
int main() {
  /*Declare float variable*/
  float a, i;

  printf("Enter a floating point number:\n");

  /*Input the float type input using scanf*/
  scanf("%f", &a); //where %f/%F/%e/%E/%g/%G/%a/%A are format specifier
  printf("%f\n", a); /*rounded and converted to decimal notation
		     in the style [-]ddd.ddd */

  scanf("%F", &a);
  printf("%F\n", a);

  scanf("%e", &i);
  printf("%e\n", i); //rounded and converted in the style [-]d.ddde±dd

  scanf("%E", &i);
  printf("%E\n", i); //exponent e is introduced as E on style [-]d.ddde±dd

  scanf("%g", &i);
  printf("%g\n", i); //same as input

  scanf("%G", &i);
  printf("%G\n", i); //same as input

#if 0
  // Used in stdc99 compilers
  scanf("%a", &i);
  printf("%a\n", i); /*converted to  hexadecimal  notation  (using
              the letters abcdef) in the style [-]0xh.hhhhp± */

  scanf("%A", &i);
  printf("%A\n", i); /*converted to  hexadecimal  notation  (using
              the letters abcdef) in the style [-]0xH.HHHHP± */

#endif
  printf("Size of float: %ld\n", sizeof(a));
  printf("Address %p\n", &a);

  printf("Range of float\npositive\t%e to %e\nnegative\t%e to %e\n",
	 FLT_MIN, FLT_MAX, -FLT_MIN, -FLT_MAX);
  printf("Range of float %f to %f\n", FLT_MIN, FLT_MAX);
  printf("Range of float %g to %g\n", FLT_MIN, FLT_MAX);
  printf("Range of float %E to %E\n", FLT_MIN, FLT_MAX);
  return 0;
}
