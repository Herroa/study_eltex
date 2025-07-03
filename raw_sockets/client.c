#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define MSG_MAX_LEN 512
#define SERVER_PORT 12345
#define SERVER_IP "127.0.0.1"
#define CLOSE_MSG "__CLOSE__"

unsigned short checksum(unsigned short *buf, int nwords) {
  unsigned long sum = 0;
  for (; nwords > 0; nwords--) sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}

int sockfd;
uint16_t client_port;

void send_close_message() {
  char msg[] = CLOSE_MSG;
  int msg_len = strlen(msg);
  char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + MSG_MAX_LEN];
  memset(packet, 0, sizeof(packet));
  struct iphdr *iph = (struct iphdr *)packet;
  struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));
  char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
  memcpy(data, msg, msg_len);
  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 0;
  iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + msg_len);
  iph->id = htons(rand() % 65535);
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;
  iph->saddr = inet_addr("127.0.0.1");
  iph->daddr = inet_addr(SERVER_IP);
  iph->check = checksum((unsigned short *)iph, sizeof(struct iphdr) / 2);
  udph->source = htons(client_port);
  udph->dest = htons(SERVER_PORT);
  udph->len = htons(sizeof(struct udphdr) + msg_len);
  udph->check = 0;
  struct sockaddr_in daddr;
  memset(&daddr, 0, sizeof(daddr));
  daddr.sin_family = AF_INET;
  daddr.sin_addr.s_addr = iph->daddr;
  daddr.sin_port = udph->dest;
  sendto(sockfd, packet, sizeof(struct iphdr) + sizeof(struct udphdr) + msg_len,
         0, (struct sockaddr *)&daddr, sizeof(daddr));
  printf("Sent close message to server. Exiting.\n");
}

void handle_sigint(int sig) {
  send_close_message();
  close(sockfd);
  exit(0);
}

int main(int argc, char *argv[]) {
  signal(SIGINT, handle_sigint);

  srand(time(NULL) ^ getpid());
  client_port = 20000 + rand() % 40000;
  printf("Client source port: %u\n", client_port);

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  int one = 1;
  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  printf("Enter your message (type 'exit' or empty line to quit):\n");
  char input[MSG_MAX_LEN + 1];
  while (1) {
    printf("> ");
    fflush(stdout);
    if (!fgets(input, sizeof(input), stdin)) {
      printf("Input error.\n");
      break;
    }
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') input[len - 1] = '\0';
    if (strlen(input) == 0 || strcmp(input, "exit") == 0) {
      break;
    }
    int msg_len = strlen(input);
    if (msg_len > MSG_MAX_LEN) msg_len = MSG_MAX_LEN;

    char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + MSG_MAX_LEN];
    memset(packet, 0, sizeof(packet));
    struct iphdr *iph = (struct iphdr *)packet;
    struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));
    char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    memcpy(data, input, msg_len);

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len =
        htons(sizeof(struct iphdr) + sizeof(struct udphdr) + msg_len);
    iph->id = htons(rand() % 65535);
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    iph->saddr = inet_addr("127.0.0.1");
    iph->daddr = inet_addr(SERVER_IP);
    iph->check = checksum((unsigned short *)iph, sizeof(struct iphdr) / 2);

    udph->source = htons(client_port);
    udph->dest = htons(SERVER_PORT);
    udph->len = htons(sizeof(struct udphdr) + msg_len);
    udph->check = 0;
    struct sockaddr_in daddr;
    memset(&daddr, 0, sizeof(daddr));
    daddr.sin_family = AF_INET;
    daddr.sin_addr.s_addr = iph->daddr;
    daddr.sin_port = udph->dest;

    if (sendto(sockfd, packet,
               sizeof(struct iphdr) + sizeof(struct udphdr) + msg_len, 0,
               (struct sockaddr *)&daddr, sizeof(daddr)) < 0) {
      perror("sendto");
      continue;
    }

    char buf[2048];
    while (1) {
      ssize_t n = recv(sockfd, buf, sizeof(buf), 0);
      if (n < 0) {
        perror("recv");
        break;
      }
      struct iphdr *iph_r = (struct iphdr *)buf;
      if (iph_r->protocol != IPPROTO_UDP) continue;
      struct udphdr *udph_r = (struct udphdr *)(buf + iph_r->ihl * 4);
      if (udph_r->dest != udph->source) continue;
      char *reply = buf + iph_r->ihl * 4 + sizeof(struct udphdr);
      int reply_len = ntohs(udph_r->len) - sizeof(struct udphdr);
      printf("Reply from server: %.*s\n", reply_len, reply);
      break;
    }
  }
  send_close_message();
  close(sockfd);
  return 0;
}