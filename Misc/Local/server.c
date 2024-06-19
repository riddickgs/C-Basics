
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int main() {
   int sfd, acpfd[10], c=0;

   sfd = getSocket(AF_INET, SOCK_STREAM, LOCALADDR, PORT, 0);
   if (sfd < 0) 
   {
      printf("[Error] %s: Failed to get socket\n", __FUNCTION__);
      return sfd;
   }
   if (listenConn(sfd, 10)) 
   {
      printf("[Error] %s: Failed to listen on socket %d\n", __FUNCTION__, sfd);
      return sfd;
   }

   char buf[100];
   while (1) {
     if ((acpfd[c%10] = acceptConn(sfd))< 0)
     {
        printf("[Error] %s: Failed to accept on socket %d\n", __FUNCTION__, sfd);
	close(sfd);
        return acpfd[c];
     }
     memset(buf, 0, sizeof(buf));
     read(acpfd[c], buf, sizeof(buf));
     if (!strncmp(buf, "exit", 4)) { close(acpfd[c]); break;}
     printf("Server received %s\n", buf);
     close(acpfd[c]);
     c++;
   }
   close(sfd);
   return 0;
}
