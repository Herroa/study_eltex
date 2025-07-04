#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCK_PATH "/tmp/mysocket"

int main() {
  int sockfd;
  struct sockaddr_un servaddr = {0}, clientaddr = {0};
  char msg[] = "Hello, server!";
  char buf[100];

  sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  clientaddr.sun_family = AF_UNIX;
  snprintf(clientaddr.sun_path, sizeof(clientaddr.sun_path),
           "/tmp/mysocket_client_%d", getpid());
  unlink(clientaddr.sun_path);
  if (bind(sockfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) < 0) {
    perror("bind");
    goto cleanup;
  }

  servaddr.sun_family = AF_UNIX;
  strncpy(servaddr.sun_path, SOCK_PATH, sizeof(servaddr.sun_path) - 1);
  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("connect");
    goto cleanup;
  }

  if (send(sockfd, msg, strlen(msg), 0) < 0) {
    perror("send");
    goto cleanup;
  }

  int n = recv(sockfd, buf, sizeof(buf) - 1, 0);
  if (n < 0) {
    perror("recv");
    goto cleanup;
  }
  buf[n] = '\0';
  printf("Reply: %s\n", buf);

cleanup:
  close(sockfd);
  unlink(clientaddr.sun_path);
  return 0;
}