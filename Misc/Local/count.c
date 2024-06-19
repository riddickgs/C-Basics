#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 20000000
#define THREADS 2

long long sum = 0;
pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;

void counting_func(int offset)
{
   for(int i=0; i< MAX; i++) {
     sum += offset;
   }
}

void *counting_thread(void *offset_ptr)
{
   int *off_ptr = (int *)offset_ptr;
   int offset = *off_ptr;
   for (int i = 0; i < MAX; i++) {
     pthread_mutex_lock(&mt); 
     sum += offset;
     pthread_mutex_unlock(&mt);
   }
   pthread_exit(NULL);
}

int main()
{
   pthread_t tid[THREADS];
   pthread_attr_t attr;
   pthread_attr_init(&attr);

   int offset1 = 1;
   //counting_func(offset);
   pthread_create(&tid[0], &attr, counting_thread, &offset1);
   int offset2 = -1;
   pthread_create(&tid[1], &attr, counting_thread, &offset2);
   //counting_func(offset);

   pthread_join(tid[0], NULL);
   pthread_join(tid[1], NULL);
   printf("Sum = %lld\n", sum);
   pthread_attr_destroy(&attr);

   return 0;
}
