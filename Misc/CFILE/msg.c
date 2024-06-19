/* This is a reader program to read the msgs received*/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

/* Message queue is limited in size as queue is maintained
 * Any process can participate in message queue which have mqd_t id
 * this is one way pipe which will send message and will be received
 * pipes needs cooperation on either side to re-establish but msg queue none
 * message queue are message oriented no care has to be taken to get,
 * say 5th message of the queue
 * message queue is used to transfer messages which have a type and size
 * */
#define SAMPLEMQ "/riddick"
#define TOT 5
int main() {
  char *name = SAMPLEMQ;
  int flags = O_RDONLY;
  unsigned int prio = 0, msgc = TOT;
  struct timespec Timeout = {time(0)+1, 0};
  char msg[48] = {0};

  /* Open msg queue id here */
  mqd_t mqid = mq_open(name, flags);
  if (mqid == -1) {
    printf("[ERROR] failed to open mq %s\n", strerror(errno));
    return mqid;
  }


  do
  {
     /* Receive message from the clients here */
     if (-1 != mq_timedreceive(mqid, msg, sizeof(msg), &prio, &Timeout)) {
	     printf("received msg [%s]\n", msg);
	     --msgc;
     }
     memset(msg, 0, sizeof(msg));
  }while (!msgc);

  mq_close(mqid);
  mq_unlink(name);
  printf("Number of msgs received are %d\n", TOT-msgc);
  return 0;
}
