/* \File:        enum.c
 *
 * \Author:      Riddickgs
 *
 * \Description: enum is used to create CONSTANTS in C
 *
 */

#include <stdio.h>
#include <stdlib.h>
/* This is annonymous enum*/

enum {
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  TEN
};

/* Annonymous enum with typedef */
typedef enum {
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY,
  SUNDAY
} DAYS;

/* Declaring constants with different values */
enum cars { TOYOTA=56, MARUTI=102, HONDA=111, TATA=23, HYUNDAI=98, KIA=45 };

/* Declaring constants starting with a value */
enum table {
  const1=100,
  const2,
  const3,
  const4,
	  const5=300,
	  const6,
	  const7
};

int main() {
    printf("NUMBERS\t%d %d %d %d %d %d %d %d %d %d\n",
      ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN);
    printf("DAYS\t%d %d %d %d %d %d %d\n",
      MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY);
    printf("CARS\t%d %d %d %d %d %d\n",
      TOYOTA, MARUTI, HONDA, TATA, HYUNDAI, KIA);
    printf("Table\t%d %d %d %d %d %d %d\n", const1, const2, const3,
		    const4, const5, const6, const7);

  return 0;
}
