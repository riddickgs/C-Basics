#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
/******************** DEFINE SECTION ************************/
#define USAGE "%s <num> \n"

/******************** Declarations *************************/
void *thread_func(void*);

struct cou {
   long long lim;
   long long sum;
};

int main(int argc, char *argv[])
{
   if (argc < 2) {
      printf(USAGE, argv[0]);
      exit(EXIT_FAILURE);
   }

   pthread_t thid;
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   printf("pid %d\n", getpid());
   fflush(stdout);
   sleep(10);

   long long *lim = (long long *)malloc(sizeof(*lim));
   *lim = atoll(argv[1]);
   pthread_create(&thid, &attr, thread_func, lim);

   long long *s;
   pthread_join(thid, (void **)&s);
   printf("This is a demo thread to work on sum %lld\n", *s);

   return 0;
}

//Thread calling function
void *thread_func(void *args)
{
   long long *lim = (long long *)args;
   long long s = 0;
   for (long long i = 0; i <= *lim; i++) {
      s += i;
   }
   long long *ans = (long long *)malloc(sizeof(*ans));
   *ans = s;
   pthread_exit(ans);
}
