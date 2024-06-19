#include <stdio.h>
#include <stdlib.h>

#define containerof(ptr,type,member) ((type*)((char*)ptr - (char *)&(((type *)0)->member)))
struct me {
  char name[20];
  int age;
  float weight;
  char color;
  int empid;
};

int main() {

	struct me mm = {
	.name = "GAurav Sharma",
	.age = 30,
	.weight = 75,
	.color = 'A',
	.empid = 5031
	};
	containerof(&mm, struct me, color);
	for (;;)
	{
	   printf("%X ", ptr++);
	   if (ptr == &mm.color) break;
	}
	printf("%p\n", &mm);
	printf("%p\n", &mm.color);
	printf("%p\n", containerof(&mm, struct me, color));
  return 0;
}


/* Swap two numbers using without third
 * x = 2 y = 3  x = x+y = 5
 *   y = x - y = 5-3 = 2
 *   x = x-y = 5-2 = 3
 *   */
