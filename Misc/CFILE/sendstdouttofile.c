#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
  int nwfd;
  printf("Starting the program\n");
  printf("Opening the file\n");
  int fd  = open("author.txt", O_RDWR, S_IRWXU,S_IRWXG,S_IROTH);
  if (fd < 0) {
    perror("open");
    return fd;
  }
  printf("Seeking the file (%d)\n", fd);
  lseek(fd, 0, SEEK_END);

  /*
  close(1);
  nwfd = dup(fd);
  replace with */
  nwfd = dup2(fd, 1);

  printf("New fd is (%d)\n", nwfd);
  printf("Exiting the program\n");

  return 0;
}
