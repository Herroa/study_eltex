#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/mysocket_stream"

int main() {
  int sockfd;
  struct sockaddr_un servaddr = {0};
  char msg[] = "Hello, server!";
  char buf[100];

  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  servaddr.sun_family = AF_UNIX;
  strncpy(servaddr.sun_path, SOCK_PATH, sizeof(servaddr.sun_path) - 1);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    close(sockfd);
    exit(1);
  }

  if (send(sockfd, msg, strlen(msg), 0) < 0) {
    perror("send");
    close(sockfd);
    exit(1);
  }

  int n = recv(sockfd, buf, sizeof(buf) - 1, 0);
  if (n >= 0) {
    buf[n] = '\0';
    printf("Reply: %s\n", buf);
  }

  close(sockfd);
  return 0;
}