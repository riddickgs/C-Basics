#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

   pid_t pid = fork();
   if (pid == 0) {
     printf("Child process\n");
     printf("Pid of the process %d\n", getpid());
     printf("Pid of the Parent process %d\n", getppid());
     char *const enp[] = {
      "/home/linux/CodePractice/CFILE/me", "Hello"
     };
     execl(enp[0], enp[0], enp[1], (char *)0);
   } else {
     getchar();
     printf("Parent process\n");
     printf("Pid of the process %d\n", getpid());
     printf("Pid of the Parent process %d\n", getppid());
   }
   return 0;
}
