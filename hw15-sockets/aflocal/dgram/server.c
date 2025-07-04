#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/mysocket"

int main() {
  int sockfd;
  struct sockaddr_un servaddr = {0}, cliaddr = {0};
  char buff[100];
  socklen_t len;

  sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  unlink(SOCK_PATH);

  servaddr.sun_family = AF_UNIX;
  strncpy(servaddr.sun_path, SOCK_PATH, sizeof(servaddr.sun_path) - 1);
  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    close(sockfd);
    unlink(SOCK_PATH);
    exit(1);
  }

  while (1) {
    len = sizeof(cliaddr);
    int n = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&cliaddr,
                     &len);
    if (n < 0) break;
    buff[n] = '\0';
    printf("Received message: %s\n", buff);
    buff[0] = 'A';
    sendto(sockfd, buff, n, 0, (struct sockaddr *)&cliaddr, len);
  }

  close(sockfd);
  unlink(SOCK_PATH);
  return 0;
}