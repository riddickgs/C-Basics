/* create a program where thread1 having ABCD and thread2 is having 1234
 * Program should print output A1B2C3D4
 * */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int done = 0;
void *thread_callback1(void *arg) {
  char *ptr = (char *)arg;
  int c = 0;
  usleep(10);
  //printf("Thread callback %p %s\n", ptr, ptr);
  while (c < 4) {
     pthread_mutex_lock(&mutex);
     printf("%c", ptr[c++]);
     done = 1;
     pthread_cond_signal(&cond);
     pthread_mutex_unlock(&mutex);
     //usleep(100);
  }

  pthread_exit(0);
}

void *thread_callback2(void *arg) {
  char *ptr = (char *)arg;
  int c = 0;
  //printf("Thread callback %p %s\n", ptr, ptr);
  while (c < 4) {
     pthread_mutex_lock(&mutex);
     if (done == 0)
       pthread_cond_wait(&cond, &mutex);
     printf("%c", ptr[c++]);
     pthread_mutex_unlock(&mutex);
     //usleep(100);
  }

  pthread_exit(0);

}

int main() {
  pthread_t p1, p2;
  char string1[5] = {'A', 'B', 'C', 'D', '\0'};
  char string2[5] = {'1', '2', '3', '4', '\0'};
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  p1 = pthread_create(&p1, NULL, thread_callback1, &string1[0]);
  p2 = pthread_create(&p2, NULL, thread_callback2, &string2[0]);
  usleep(3000);

  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
  return 0;
}
