#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sockfd;
  struct sockaddr_in servaddr;
  char msg[100] = "Hello, server!";
  char buf[100];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(8080);

  connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

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