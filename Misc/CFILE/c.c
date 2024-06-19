#include <stdio.h>
#include <string.h>
#include <unistd.h>

int my_atoi(char *);
char *my_strtok(char *, char *);

struct var {
  int i;
  const int j;
};
int main()
{
  char str[] = "ABC:cd,lmno!pqr$wxyz";
  struct var v = {
    .i = 10,
    .j = 20,
  };
  printf("i %d j %d\n", v.i, v.j);
  //ptr = str;
  printf("1 %s\n", strtok(str, ":"));
  printf("2 %s\n", strtok(NULL, ","));
  printf("3 %s\n", strtok(NULL, "!"));
  printf("4 %s\n", strtok(NULL, "$"));
  printf("5 %s\n", strtok(NULL, "\0"));
  printf("\n");
  char buf[10] = "Helooo!!\n";

  write(1, buf, strlen(buf));
  fprintf(stderr, "stderr - %s", buf);

  printf("atoi %d\n", my_atoi("1242asnsk"));
  return 0;
}

//local atoi
int my_atoi(char *buf) {
  long int res = 0;
  if (!buf) return 0;
  char *ptr = buf;
  while (*ptr) { // first 1 then 1 * 10, res 1|res 12|res 124|res 1242
    if (*ptr >= '0' && *ptr <= '9') {
       res = res * 10 + (*ptr-'0');
    }
    ptr++;
  }
  return res;
}

char *my_strtok(char *str, char *del) {


  return str;
}
