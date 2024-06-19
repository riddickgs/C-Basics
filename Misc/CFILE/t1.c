#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

static int CheckSum(unsigned short *buf, int sz)
{
   int nleft = sz;
   int sum = 0;
   unsigned short *w = buf;
   unsigned short ans = 0;

   while (nleft > 1)
   {
      sum += *w++;
      nleft -= 2;
   }

   if (nleft == 1)
   {
      *(unsigned char *) (&ans) = *(unsigned char *) w;
      sum += ans;
   }

   sum = (sum >> 16) + (sum & 0xFFFF);
   sum += (sum >> 16);
   ans = ~sum;
   return (ans);
}

FILE* (*openFile)(const char *, const char *);
//get a function pointer which will give File pointer to open file
FILE* openf(const char *buf, const char *mode) {
  FILE *fp = fopen(buf, mode);
  if (!fp) {
    perror("Failed to open file");
  }
  return fp;
}
typedef unsigned char BYTE;
#define LOBYTE(w) ((BYTE)w)
// I have a function which needs to be called 100 times
int main() {
  //create multiple threads and process the communication
  unsigned char b = 1;
  unsigned int i = (b <<12) + (b <<8)+1;
  printf("%X\n", i);
  void *fff = &openf;
  openFile = (FILE *(*)(const char *, const char *))fff;
  FILE *f = (*openFile)("me.txt", "w+");
  if (f) fclose(f);

  char *buf = "la catte! ola vessitio!";
  printf("CS = %d\n", CheckSum((unsigned short *)buf, strlen(buf)));
  return 0;
}
