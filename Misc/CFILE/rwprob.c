/* This is a story
 * writer threads will take input from external sources
 * likely drivers (serialized) and write to a shared buffer
 * reader threads process it to the application
 * */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define M 10
#define N 20
#define BUFSZ 100
#define DELAY 10

pthread_t readerthreads[N], writerthreads[M];
pthread_mutex_t mutexlock;
pthread_cond_t canread;
pthread_cond_t canwrite;

int rcnt = 0;
int wcnt = 0;
int waitr = 0;
int waitw = 0;
char data[100] = "AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFFGGGGGGGGGGIIIIIIIIIIHHHHHHHHHHJJJJJJJJJJ";
char buffer[BUFSZ] = {0};

typedef struct {
   int fd;
   int size;
   int state;
}_DT;

void *getDeviceData(int deviceId, int size) 
{
  /*here deviceId is starting offset of the 
   * driver data from data buffer
   */
   return &data[deviceId*size];
}
///data came from the driver single if multiple
//this will take data from a external driver and place it on the share memory
int get_external_data(char *buffer, int bufferSizeInBytes, int idx)
{
  //Copy data from the device driver and keep in shared buffer
  void *copy = getDeviceData(idx, bufferSizeInBytes);
  memcpy(buffer, copy, bufferSizeInBytes);
  return 0;
}

void printBuf(char *buf, int size) {
  char bf[size+1];
  memcpy(bf, buf, size);
  bf[size] = 0;
  printf("data %s\n", bf);
}

//this will read the data from the share memory
void process_data(char *buffer, int bufferSizeInBytes)
{
   //use the share memory buf to print
   if (!buffer || !*buffer) {
     printf("NO data to read\n");
     return;
   }
   printBuf(buffer, bufferSizeInBytes);
}

void beginRead(int i)
{
  pthread_mutex_lock(&mutexlock);
  //if there are active or waiting writer
  if (wcnt == 1 || waitw > 0) {
    //incrementing the waiting reader
    waitr++;
    //suspend the reader
    pthread_cond_wait(&canread, &mutexlock);
    waitr--;
  }

  rcnt++;
  //Do the reader Task
  printf("Reader %d is reading\n", i);
  process_data(&buffer[(i%10)*10], BUFSZ/M);
  pthread_mutex_unlock(&mutexlock);
  pthread_cond_broadcast(&canread);
}

void endRead(int i)
{
  pthread_mutex_lock(&mutexlock);
  if (--rcnt == 0) //if no readers left then writer enters
     pthread_cond_signal(&canwrite);
  pthread_mutex_unlock(&mutexlock);
}

void beginWrite(int i) 
{
  pthread_mutex_lock(&mutexlock);
  //a writer can enter when there is no active or waiting readers or other writers
  if (wcnt == 1 || rcnt > 0) {
    ++waitw;
    pthread_cond_wait(&canwrite, &mutexlock);
    --waitw;
  }
  wcnt = 1;
  get_external_data(&buffer[i*10], BUFSZ/M, i);
  printf("Writer %d is writing\n", i);
  pthread_mutex_unlock(&mutexlock);

}

void endWrite(int i) {

  pthread_mutex_lock(&mutexlock);
  wcnt = 0;
  //if any readers are waiting, threads are unblocked
  if (waitr > 0)
     pthread_cond_signal(&canread);
  else
     pthread_cond_signal(&canwrite);
  pthread_mutex_unlock(&mutexlock);
}

void *reader_thread(void *arg1) {
  int arg = (int)*(int*)arg1;
  int c = 0;
  while (c < N) {
    usleep(DELAY);
    beginRead(arg);
    endRead(arg);
    c++;
  }

  return NULL;
}

void *writer_thread(void *arg1) {

  int arg = (int)*(int*)arg1;
  int c = 0;

  while (c < M) {
    usleep(DELAY);
    beginWrite(arg);
    endWrite(arg);
    c++;
  }

  return NULL;
}

int main(int argc, char *argv[])
{
   int i;
   int a1[M], a2[N];

   pthread_mutex_init(&mutexlock, NULL);
   pthread_cond_init(&canread, NULL);
   pthread_cond_init(&canwrite, NULL);

   for (i = 0; i < M; i++){
     a1[i] = i;
     if (pthread_create(&writerthreads[i], NULL, writer_thread, &a1[i])) {
       perror("pthread_create");
       continue;
     }
   }

   for (i = 0; i < N; i++){
     a2[i] = i;
     if (pthread_create(&readerthreads[i], NULL, reader_thread, &a2[i])) {
       perror("pthread_create");
       continue;
     }
   }

   for (i = 0; i < N; i++){
     pthread_join(readerthreads[i], NULL);
   }

   for (i = 0; i < M; i++){
     pthread_join(writerthreads[i], NULL);
   }

   pthread_mutex_destroy(&mutexlock);
   pthread_cond_destroy(&canread);
   pthread_cond_destroy(&canwrite);
   return 0;
}
