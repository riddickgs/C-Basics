/* This program is to test the comma separated input
 * and separate them into an array using sscanf*/

#include <stdio.h>
#include <stdlib.h>

int main() {
  char ab[6][16] = {0}, buffer[] = "My,name,is,anthony,gonsalves,and,something";

  int ret = sscanf(buffer, "%[^','],%[^','],%[^','],%[^','],%[^','],%s",
		  (char *)&ab[0],(char *) &ab[1], (char *)&ab[2],(char *) &ab[3],(char *) &ab[4], (char *) &ab[5]);

  printf("%s %s %s %s %s %s\n", ab[0], ab[1], ab[2], ab[3], ab[4], ab[5]);

  printf("%s\n", &buffer[10]);

  return 0;
}
