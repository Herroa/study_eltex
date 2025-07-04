#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sockfd, connfd;
  struct sockaddr_in servaddr, cliaddr;
  char buff[100];
  socklen_t len;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(8080);

  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    close(sockfd);
    exit(1);
  }

  if (listen(sockfd, 5) < 0) {
    perror("listen");
    close(sockfd);
    exit(1);
  }

  printf("wаit connection...\n");
  len = sizeof(cliaddr);
  connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
  if (connfd < 0) {
    perror("accept");
    close(sockfd);
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
  return 0;
}