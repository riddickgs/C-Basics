#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>

#define FNAME "/home/linux/.myfifo"
#define TIME 5
int main() {
  int r, fifo;
  char buffer[64];
  struct timeval tv = {60, 0};
  fd_set rfds;

  /* Step 1 create the fifo/file */
  r = mknod(FNAME, 010666, 0);
  if (r < 0) {
    perror("mkfifo");
    if (errno == EEXIST) {
      unlink(FNAME);
      r = mknod(FNAME, 010666, 0);
      if (r < 0) {
        perror("mkfifo");
        return r;
      }
    } else 
      return r;
  }

  /* Step 2 open the fifo */
  fifo = open(FNAME, O_RDWR|O_NONBLOCK);
  if (fifo < 0) {
    perror("open");
    return fifo;
  }
  FD_SET(fifo, &rfds);
  for (;;) {
    r = select(fifo+1, &rfds, NULL, NULL, &tv);
    if (r) {
      read(fifo, buffer, sizeof(buffer));
      printf("buffer %s\n", buffer);
    }
    if (!r) break;
  }
  close(fifo);
  return 0;
}
