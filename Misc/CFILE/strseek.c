#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MSG "This course is all about Linux system programming. The instructor of this course is Rohan."
int main() {
  int fd = open("author.txt", O_RDWR|O_CREAT, S_IRWXU, S_IRWXG, S_IROTH), len;
  if (fd < 0) {
    perror("open");
    return fd;
  }
  char buf1[] = MSG;
  char buf[] = "Mohan.";
  len = strcspn(buf1, "Rohan");
  printf("len %d MAXLEN %ld\n", len, MAXLEN);
  exit(0);
  lseek(fd, strstr(buf1, "Rohan")-&buf1[0], SEEK_SET);
  write(fd, buf, strlen(buf));
  close(fd);
  return 0;
}
