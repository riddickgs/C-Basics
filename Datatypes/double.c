/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File		double.c
 * Author	Riddick
 * Modify	Riddick|Sat Oct  7 14:16:22 EDT 2023
 * Desc		This file contains double type input and output
 * Notes	double type includes input ranges from 
		positive	2.225074e-308 to 1.797693e+308
		negative	-2.225074e-308 to -1.797693e+308
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <float.h>

/*Mandatory main function |return type 'int' */
int main() {
  /*Declare double variable*/
  double a, i;

  printf("Enter a double input:\n");

  /*Input the double type input using scanf*/
  scanf("%lf", &a); //where %lf is the format specifier
  printf("%lf\n", a);

  scanf("%lF", &a);
  printf("%lF\n", a);

  scanf("%le", &i);
  printf("%le\n", i); //rounded and converted in the style [-]d.ddde±dd

  scanf("%lE", &i);
  printf("%lE\n", i); //exponent e is introduced as E on style [-]d.ddde±dd

  scanf("%lg", &i);
  printf("%lg\n", i); //same as input

  scanf("%lG", &i);
  printf("%lG\n", i); //same as input

#if 0
  // Used in stdc99 compilers
  scanf("%la", &i);
  printf("%la\n", i); /*converted to  hexadecimal  notation  (using
              the letters abcdef) in the style [-]0xh.hhhhp± */

  scanf("%lA", &i);
  printf("%lA\n", i); /*converted to  hexadecimal  notation  (using
              the letters abcdef) in the style [-]0xH.HHHHP± */
#endif

  printf("Size of double: %ld\n", sizeof(a));
  printf("Address %p\n", &a);

  printf("Range of double\npositive\t%le to %le\nnegative\t%le to %le\n",
         DBL_MIN, DBL_MAX, -DBL_MIN, -DBL_MAX);
  printf("Range of double %lf to %lf\n", DBL_MIN, DBL_MAX);
  printf("Range of double %lg to %lg\n", DBL_MIN, DBL_MAX);

  return 0;
}
