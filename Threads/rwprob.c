/* This is a story
 * writer threads will take input from external sources
 * likely drivers (serialized) and write to a shared buffer
 * reader threads process it to the application
 * Process became autonomous/asynchronous
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

#define M 10  /* Number of writer threads */
#define N 20  /* Number of reader threads */
#define BUFSZ 100  /* Buffer size */
#define DELAY 10   /* Delay between reader threads */

pthread_t readerthreads[N], writerthreads[M];
pthread_mutex_t mutexlock; /* Mutex variable : lock between r/w threads */
pthread_cond_t canread;    /* Conditional variable */
pthread_cond_t canwrite;   /* Conditional variable */

int rcnt = 0;  /* reader count */
int wcnt = 0;  /* writer count */
int waitr = 0; /* waiting reader count */
int waitw = 0; /* waiting writer count */
char data[100] = "AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDDEEEEEEEEEEFFFFFFFFFFGGGGGGGGGGIIIIIIIIIIHHHHHHHHHHJJJJJJJJJJ";
char buffer[BUFSZ] = {0}; /* Shared buffer */

/* Get Data from consolidated memory using device index */
void *getDeviceData(int deviceIdx, int size) 
{
   return &data[deviceIdx*size];
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

/* Copying data and print buffer as
 * no nul character present on original */
void printBuf(char *buf, int size) {
  char bf[size+1];
  memcpy(bf, buf, size);
  bf[size] = 0;
  printf("data %s\n", bf);
}

/* Processing shared data */
void process_data(char *buffer, int bufferSizeInBytes)
{
   /* No data to read then return */
   if (!buffer || !*buffer) {
     printf("NO data to read\n");
     return;
   }
   printBuf(buffer, bufferSizeInBytes);
}

/* Reading task begins */
void beginRead(int i)
{
  /* get the lock first */
  pthread_mutex_lock(&mutexlock);

  /* if there are active or waiting writer */
  if (wcnt == 1 || waitw > 0) {

    /* increment the waiting reader */
    waitr++;

    /* suspend the reader */
    pthread_cond_wait(&canread, &mutexlock);
    waitr--;
  }

  /* increment active reader */
  rcnt++;

  /* Reader Task here */
  printf("Reader %d is reading\n", i);

  /* Process data */
  process_data(&buffer[(i%10)*10], BUFSZ/M);

  /* Unlock from reader */
  pthread_mutex_unlock(&mutexlock);

  /* Broadcast signal for others */
  pthread_cond_broadcast(&canread);
}

/* No active readers then allow writers */
void endRead(int i)
{
  /* Get the lock */
  pthread_mutex_lock(&mutexlock);

  /* Check for count for active readers */
  if (--rcnt == 0)
    /* No readers left then writer enters */
     pthread_cond_signal(&canwrite);

  /* Unlock from reading end task */
  pthread_mutex_unlock(&mutexlock);
}

/* Writing task begins */
void beginWrite(int i) 
{
  /* Get the lock */
  pthread_mutex_lock(&mutexlock);

  /* A writer can enter when there is
   * no active or waiting readers or other writers */
  if (wcnt == 1 || rcnt > 0) {

    /* increment the waiting writer */
    ++waitw;

    /* suspend the writer */
    pthread_cond_wait(&canwrite, &mutexlock);

    /* decrement the waiting writer */
    --waitw;
  }

  /* Writer got a chance to write*/
  wcnt = 1;
  /* Write the data to the shared buffer with size */
  get_external_data(&buffer[i*10], BUFSZ/M, i);

  printf("Writer %d is writing\n", i);

  /* Unlock the writer */
  pthread_mutex_unlock(&mutexlock);

}

/* Writer ends task*/
void endWrite(int i) {

  /* Get the lock */
  pthread_mutex_lock(&mutexlock);

  /* No writer active */
  wcnt = 0;

  /* if any readers are waiting, threads are unblocked */
  if (waitr > 0)
     /* Signal reader to allow reading */
     pthread_cond_signal(&canread);
  else
     /* Signal writer to keep writing */
     pthread_cond_signal(&canwrite);

  /* Unlock from writer end task*/
  pthread_mutex_unlock(&mutexlock);
}

/* Reader thread callback routine */
void *reader_thread(void *arg1) {
  /* cast the void argument */
  int arg = (int)*(int*)arg1;

  /* count variable */
  int c = 0;

  /* loop N number of times */
  while (c < N) {
    /* Delay for a few moments */
    usleep(DELAY);

    /* Reading begin */
    beginRead(arg);

    /* Reading end */
    endRead(arg);

    /* increment the counter */
    c++;
  }

  /* return a non void function */
  return NULL;
}

/* Writer thread callback routine */
void *writer_thread(void *arg1) {
  /* cast the argument */
  int arg = (int)*(int*)arg1;

  /* initialize the counter */
  int c = 0;

  /* Experimental step to check even number of threads only */
  if (arg%2) pthread_cancel(pthread_self());

  /* loop M number of times */
  while (c < M) {
    /* Delay for a few moments */
    usleep(DELAY);
    
    /* Begin write */
    beginWrite(arg);
    
    /* End write */
    endWrite(arg);

    /* increment the counter */
    c++;
  }

  /* return a non-void function */
  return NULL;
}

/* Driver function */
int main(int argc, char *argv[])
{
   int i;
   int a1[M], a2[N];

   /* Mutex initializtion */
   pthread_mutex_init(&mutexlock, NULL);

   /* Read conditional initializtion */
   pthread_cond_init(&canread, NULL);

   /* Write conditional initializtion */
   pthread_cond_init(&canwrite, NULL);

   /* Create M number of threads */
   for (i = 0; i < M; i++){
     a1[i] = i;
     if (pthread_create(&writerthreads[i], NULL, writer_thread, &a1[i])) {
       perror("pthread_create");
       continue;
     }
   }

   /* Create N number of threads */
   for (i = 0; i < N; i++){
     a2[i] = i;
     if (pthread_create(&readerthreads[i], NULL, reader_thread, &a2[i])) {
       perror("pthread_create");
       continue;
     }
   }

   /* Waiting for reader threads */
   for (i = 0; i < N; i++){
     pthread_join(readerthreads[i], NULL);
   }

   /* Waiting for writer threads */
   for (i = 0; i < M; i++){
     pthread_join(writerthreads[i], NULL);
   }

   /* Check the final write */
   printf("%s\n", buffer);

   /* return to __main function */
   return 0;
}
