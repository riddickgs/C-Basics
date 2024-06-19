#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define CLIENTS 5
#define BUFSZ 100
#define MSG "Hello this is temporary data!! @ AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
typedef struct child_data {
  int child_id;
  int fd[2];
} CHDATA;

CHDATA *chid;
char buf[BUFSZ];
int counter = 0;

int process_child(int child_idx, int fd) {
  printf("child idx %d pid %d ppid %d\n", child_idx, getpid(), getppid());
  read(fd, buf, BUFSZ);
  printf("child[%d]|pid[%d]|%s\n", child_idx, getpid(), buf);
  close(fd);
  exit(0);
}

void process_parent() {
  //This is server process
  printf("Hi I am parent process pid %d\n", getpid());
  int i = 0, pid = 0, st;

  //communication with child 
  for (i = 0; i < CLIENTS; i++) {
   // printf("child pid %d", chid[i].child_id);
    sprintf(buf, "pid[%d] %s", chid[i].child_id, MSG);
    write(chid[i].fd[1], buf, strlen(buf));
    close(chid[i].fd[1]);
  }

  //Exiting child
  for (i = 0; i < CLIENTS; i++) {
    pid = wait(&st);
    fprintf(stderr, "[%d] pid exited %d status %d\n", i, pid, WEXITSTATUS(st));
  }
  free(chid);
}

void create_child_table_entry(int pid, int fd) {
  int sz = (counter+1) * sizeof(CHDATA);
  chid = realloc(chid, sz);
  chid[counter].child_id = pid;
  chid[counter].fd[1] = fd;
  counter++;
}

int main() {

  //one server and 5 clients
  int i, ret;

  for (i = 0; i < CLIENTS; i++) {
    int fd[2];
    if (pipe(fd) == -1) {
      perror("pipe");
      exit(0);
    }
    if ((ret = fork()) == 0) {
    //This is child process
      close(fd[1]); //close the write from child
      process_child(i, fd[0]);
    } else {
    //this is parent process
      close(fd[0]); //close read from parent
      create_child_table_entry(ret, fd[1]);
    }
  }

  process_parent();
  return 0;
}
