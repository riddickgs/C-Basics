#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>



//#define ADDR "127.0.0.1"
#define ADDR "0.0.0.0"
#define PORT 8080

void print_mac(const unsigned char *p) {
  printf("%X-%X-%X-%X-%X-%X\n", p[0],p[1],p[2],p[3],p[4],p[5]);
}
int init_udp(int *sid) {
  unsigned char mac[7];
  struct sockaddr_in cliadd, udpfrom;
  char buf[100];
  int rb = 0;
  *sid = socket(AF_INET, SOCK_DGRAM, 0);
  if (*sid < 0) {
    perror("[Error] Failed to create socket");
    return rb;
  }
  struct ifreq ifr;
  strcpy(ifr.ifr_name, "enp0s3");
  if ((rb = ioctl(*sid, SIOCGIFHWADDR, &ifr)) == -1) {
    perror("Failed to get hw add");
    close(*sid);
    return rb;
  }
  memcpy(&mac[0], &ifr.ifr_hwaddr.sa_data, 6);
  mac[6]=0;
  print_mac(mac);
  memset(&cliadd, 0, sizeof(cliadd));
  cliadd.sin_family = AF_INET;
  inet_aton(ADDR, (struct in_addr*)&cliadd.sin_addr.s_addr);
  cliadd.sin_port = htons(PORT);

  if((rb = bind(*sid, (struct sockaddr *)&cliadd.sin_addr.s_addr, sizeof(cliadd))) < 0) {
    perror("failed to bind");
    close(*sid);
    return rb;
  }

  socklen_t l = sizeof(udpfrom);
  rb = recvfrom(*sid, buf, sizeof(buf), 0, (struct sockaddr *)&udpfrom, &l);
  if (rb < 0) {
    perror("Error in reading data");
    close(*sid);
    return rb;
  
  }
  return rb;
}

void comm(int *sid) {
}

int end_udp(int *sid) {
  int ret = 0;
  printf("[Debug] Closing connection on %d socket\n", *sid);
  if (*sid)
    ret = close(*sid);

  return ret;
}
//server side
void *commTask(void *arg) {
  int sockid;
  if (init_udp(&sockid))
    return NULL;

  comm(&sockid);

  end_udp(&sockid);
  return NULL;
}
int main(int argc , char *argv[]) {
  pthread_t commtid;
  pthread_create(&commtid, NULL, commTask, NULL); //joinable thread
  pthread_join(commtid, NULL); //wait till thread terminates

  return 0;
}
