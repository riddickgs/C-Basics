#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * (*callback_r)(void *arg)
{
  printf("This is callback thread\n");
  pthread_exit(NULL);

}
int main() {
  pthread_t t;
  pthread_attr_t at;
  int r;

  r = pthread_create(&t, &at, callback_r, NULL);
  if (r) {
    perror("pthread_create");
    return -1;
  }
  r = pthread_join(t, NULL);
  if (r) {
    perror("pthread_join");
    return -1;
  }
  return 0; 
}
