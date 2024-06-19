#ifndef _UTILS_H_
#define _UTILS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/*  externs*/
#define LOCALADDR "0.0.0.0"
#define PORT      8080
#define SERVADDR  "127.0.0.1"
extern const int getSocket(const int, const int, const char *, const unsigned short, const int);
extern const int connectServer(const int sockfd, const char *servaddr, const unsigned short port);
extern int listenConn(int sockfd, int backlog);
extern int acceptConn(int sockfd);

#endif
