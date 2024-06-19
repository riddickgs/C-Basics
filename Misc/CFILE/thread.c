#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/times.h>


#define LOOPS 10000000
long long sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *counting_thread(void *args) {
  int *ptr = (int*)args;
  int offset = *ptr;

  for (int i =0; i < LOOPS; i++){
    pthread_mutex_lock(&mutex);
    sum+= offset;
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}

int main(){
  struct tms buf;
 // clock_t c = times(&buf);
  int offset1=1;
  pthread_t tid1;
  pthread_create(&tid1, NULL, counting_thread, &offset1);

  int offset2=-1;
  pthread_t tid2;
  pthread_create(&tid2, NULL, counting_thread, &offset2);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  printf("Sum = %lld\n", sum);
  fprintf(stderr, "time = %ld %ld\n", times(&buf), sysconf(CLOCKS_PER_SEC));
  return 0;
}
