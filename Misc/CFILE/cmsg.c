/* This is the producer program which will create msgs for the reader to read */

#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define SAMPLEMQ "/riddick"
#define SAMPLEMSG "This is a sample to check message queue"
#define FAILMQMSG "Failed to open message queue"
#define FAILSLMSG "Failed to set flag on message queue"
#define FAILUNLINK "Failed to remove message queue identifier"
int main() {
  const char *name = SAMPLEMQ;
  mqd_t mqid;
  unsigned int prio = 0;
  const char *msg = SAMPLEMSG;
  int done = 0;
  const int oFlags = O_CREAT | O_EXCL | O_RDWR;
  const int pFlags = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
  struct mq_attr attr;
  attr.mq_maxmsg = 5;
  attr.mq_msgsize = 48;

  mqid = mq_open(name, oFlags, pFlags, &attr);
  if (-1 == mqid) {
    if (EEXIST == errno) {
      if (mq_unlink(name) != 0) {
        perror(FAILMQMSG);
	return errno;
      }
    } else {
      perror(FAILMQMSG);
      return errno;
    }
    mqid = mq_open(name, oFlags, pFlags, &attr);
    if (-1 == mqid)
      return errno;
  }

  if (-1 == fcntl(mqid, F_SETFL, O_RDWR|O_EXCL)) {
    perror(FAILSLMSG);
    return errno;
  }

  do {
     if (-1 != mq_send(mqid, msg, strlen(msg), prio)) {
       done++;
     }
  
  } while (5 == done);

  mq_close(mqid);
  printf("Exiting\n");
  return 0;
}
