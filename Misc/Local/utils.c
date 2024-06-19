#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

extern const int getSocket(const int domain, const int type,
	       	const char *addr, const unsigned short port, const int protocol)
{
   int sfd;
   struct sockaddr_in cli;
   sfd = socket(domain, type, protocol);

   if (sfd < 0) {
      perror("socket()");
      return sfd; //error handled on caller
   }

   memset(&cli, 0, sizeof(cli));
   cli.sin_family = domain;
   if (addr)
     inet_aton(addr, (struct in_addr *)&cli.sin_addr.s_addr);
   else
     cli.sin_addr.s_addr = INADDR_ANY;
   if (port) cli.sin_port = htons(port);

   if (bind(sfd, (struct sockaddr *)&cli, sizeof(cli))) {
      perror("bind()");
      close(sfd);//should not leak fd
      return -1;
   }

   printf("Socket created successfully %d\n", sfd);
   return sfd;
}

extern const int connectServer(const int sockfd, const char *servaddr, const unsigned short port)
{
   struct sockaddr_in saddr;
   memset(&saddr, 0, sizeof(saddr));
   saddr.sin_family = AF_INET;
   saddr.sin_addr.s_addr = inet_addr(servaddr);
   saddr.sin_port = htons(port);
   if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
      perror("connect()");
      return -1;
   }

   printf("connected successfully\n");
   return 0;
}

extern int listenConn(int sockfd, int backlog)
{
   return listen(sockfd, backlog);
}

extern int acceptConn(int sockfd)
{
   int afd;
   struct sockaddr_in addr;
   unsigned int len = sizeof(addr);
   memset(&addr, 0, sizeof(addr));
   afd = accept(sockfd, (struct sockaddr *)&addr, &len);
   printf("Accept connection from client %s[FD-%d]\n", inet_ntoa(addr.sin_addr), afd);
   return afd;
}
