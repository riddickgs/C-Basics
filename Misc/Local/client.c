#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#define FIRSTMSG "Hello server! How are you today?"
enum {
   TCP,
   UDP,
   RAW
};

int main(int argc, char **argv)
{
   int sfd, ret;
   sfd = getSocket(AF_INET, SOCK_STREAM, SERVADDR, 0, TCP);
   //sfd = getSocket(AF_INET, LOCALADDR, port, UDP);
   if (sfd < 0) {
     printf("[Error]: %s failed to get socket\n", __FUNCTION__);
     return -1;
   }

   ret = connectServer(sfd, SERVADDR, 8080);
   if (ret < 0) {
     printf("[Error] Failed to connect to server\n");
     close(sfd);
     return -1;
   }

   if (argc == 2)
     write(sfd, argv[1], strlen(argv[1]));
   else
     write(sfd, FIRSTMSG, strlen(FIRSTMSG));

   close(sfd);
   return 0;
}
