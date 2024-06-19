#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
/******************** DEFINE SECTION ************************/
#define USAGE "%s <num1> <num2> <num3> ... <num N>\n"

/******************** Declarations *************************/
void *thread_func(void*);

struct cou {
   long long lim;
   long long sum;
};

int main(int argc, char *argv[])
{
   if (argc < 1) {
      printf(USAGE, argv[0]);
      exit(EXIT_FAILURE);
   }
   int nargs = argc-1;
   pthread_t thid[nargs];
   pthread_attr_t attr;
   pthread_attr_init(&attr);

   struct cou args[nargs];
   for (int a = 0; a < nargs; a++) {
     args[a].lim = atoll(argv[a+1]);
     pthread_create(&thid[a], &attr, thread_func, &args[a]);
   }

   for (int a=0; a < nargs; a++) {
     pthread_join(thid[a], NULL);
     printf("This is a demo thread %d to work on sum %lld\n", a, args[a].sum);
   }

   return 0;
}

//Thread calling function
void *thread_func(void *args)
{
   struct cou *st = (struct cou *)args;
   long long s = 0;
   for (long long i = 0; i <= st->lim; i++) {
      s += i;
   }
   st->sum = s;
   pthread_exit(0);
}
