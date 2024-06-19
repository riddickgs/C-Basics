#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  FILE *fp;
  int size;

  if (argc != 2 || !argv[1]) {
    printf("Missing file name to calculate its size\n");
    return -1;
  }

  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    perror("fopen");
    return -1;
  }

  clock_t c = clock();
#ifdef STAT
  struct stat s;
  stat(argv[1], &s);
  size = s.st_size;
#else
  int prev = ftell(fp);
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);
  fseek(fp, prev, SEEK_SET);
#endif
  printf("File Size %d, ticks = %lu\n", size, clock()-c);

  return 0;
}
