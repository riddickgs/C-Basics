#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define FNAME "/home/linux/.myfifo"
#define USAGE "npipr <message> <message>..\n"
#define BUFSZ 64
int main(int argc, char *argv[]) {
  //This is Named pipe program
  int r, fifo;
  char buffer[BUFSZ];
  if (argc < 2) {
    printf(USAGE);
    return -1;
  }
  /* Step 2 open the fifo */
  fifo = open(FNAME, O_WRONLY|O_NONBLOCK);
  if (fifo < 0) {
    perror("open");
    return fifo;
  }
  for (r = 0; r < argc; r++) {
    strncpy(buffer, argv[r], BUFSZ-1);
    buffer[BUFSZ-1]='\0';
    /* step 3 write to file */
    write(fifo, buffer, sizeof(buffer));
  }
  /* step 4 close the file */
  close(fifo);
  return 0;
}
