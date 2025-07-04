#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;
  char buff[100];
  socklen_t len;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    exit(1);
  }

  len = sizeof(cliaddr);
  while (1) {
    int n = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *)&cliaddr,
                     &len);
    buff[n] = '\0';
    printf("Received message: %s\n", buff);
    buff[0] = 'A';
    sendto(sockfd, buff, n, 0, (const struct sockaddr *)&cliaddr, len);
  }

  close(sockfd);
  return 0;
}