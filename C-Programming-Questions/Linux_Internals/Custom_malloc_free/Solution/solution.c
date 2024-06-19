/*
Custom malloc() and free()
                                                                                                                            WTD: Implement your own malloc() and free() functions. Use system calls like sbrk() to manage memory allocation. 
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *my_malloc(const size_t size) {
  //calling system call
  return sbrk(size);
}

void my_free(void *ptr) {

}

int main() {

  //custom malloc
  char *ptr = my_malloc(10*sizeof(char));
  strcpy(ptr, "function");
  printf("OUT:%s\n", ptr);
  return 0;
}
