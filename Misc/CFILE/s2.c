#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int
main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    /* Child reads from pipe */
        fd_set rfds;
	int ret;
	//struct timeval tv = {.tv_sec = 2, .tv_usec = 0};
	FD_ZERO(&rfds);
	FD_SET(pipefd[0], &rfds);
	ret = select(pipefd[0]+1, &rfds, NULL, NULL, NULL);
	if (ret == -1)
          perror("select");
	else if (ret)
          printf("Data is available\n");
	else
          printf("data unavailable until 2 sec\n");
        close(pipefd[1]);          /* Close unused write end */

        while (FD_ISSET(pipefd[0], &rfds) && read(pipefd[0], &buf, 1) > 0) //reads one byte at a time
            write(STDOUT_FILENO, &buf, 1);   //writes one byte at a time to stdout

        write(STDOUT_FILENO, "\n", 1);       //writes new line to stdout
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);

    } else {            /* Parent writes argv[1] to pipe */
        close(pipefd[0]);          /* Close unused read end */
        write(pipefd[1], argv[1], strlen(argv[1]));
        close(pipefd[1]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    }
}
