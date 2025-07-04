#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/mysocket_stream"

int main() {
  int sockfd, connfd;
  struct sockaddr_un servaddr = {0}, cliaddr = {0};
  char buff[100];
  socklen_t len;

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
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
    exit(1);
  }

  if (listen(sockfd, 5) < 0) {
    perror("listen");
    close(sockfd);
    unlink(SOCK_PATH);
    exit(1);
  }

  printf("wait connection...\n");
  len = sizeof(cliaddr);
  connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
  if (connfd < 0) {
    perror("accept");
    close(sockfd);
    unlink(SOCK_PATH);
    exit(1);
  }
  printf("client connected!\n");

  while (1) {
    int n = recv(connfd, buff, sizeof(buff) - 1, 0);
    if (n <= 0) break;
    buff[n] = '\0';
    printf("Received message: %s\n", buff);
    buff[0] = 'A';
    send(connfd, buff, n, 0);
  }

  close(connfd);
  close(sockfd);
  unlink(SOCK_PATH);
  return 0;
}