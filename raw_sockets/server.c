#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 65536
#define MAX_CLIENTS 100
#define MSG_MAX_LEN 512
#define SERVER_PORT 12345
#define SERVER_IP "127.0.0.1"
#define CLOSE_MSG "__CLOSE__"

struct client_info {
  struct in_addr addr;
  uint16_t port;
  int msg_count;
};

struct client_info clients[MAX_CLIENTS];
int client_count = 0;

int get_client_index(struct in_addr addr, uint16_t port) {
  for (int i = 0; i < client_count; ++i) {
    if (clients[i].addr.s_addr == addr.s_addr && clients[i].port == port) {
      return i;
    }
  }
  if (client_count < MAX_CLIENTS) {
    clients[client_count].addr = addr;
    clients[client_count].port = port;
    clients[client_count].msg_count = 0;
    return client_count++;
  }
  return -1;
}

unsigned short checksum(unsigned short *buf, int nwords) {
  unsigned long sum = 0;
  for (; nwords > 0; nwords--) sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (unsigned short)(~sum);
}

int main() {
  int sockfd;
  char buffer[BUF_SIZE];
  struct sockaddr_in saddr, daddr;
  int one = 1;

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  printf("Echo server on raw UDP sockets started! Listening on port %d\n",
         SERVER_PORT);

  while (1) {
    ssize_t data_size = recv(sockfd, buffer, BUF_SIZE, 0);
    if (data_size < 0) {
      perror("recv");
      continue;
    }

    struct iphdr *iph = (struct iphdr *)buffer;
    if (iph->protocol != IPPROTO_UDP) continue;

    struct udphdr *udph = (struct udphdr *)(buffer + iph->ihl * 4);
    if (ntohs(udph->dest) != SERVER_PORT) continue;
    char *msg = buffer + iph->ihl * 4 + sizeof(struct udphdr);
    int msg_len = ntohs(udph->len) - sizeof(struct udphdr);
    if (msg_len <= 0 || msg_len > MSG_MAX_LEN) continue;

    struct in_addr client_addr;
    client_addr.s_addr = iph->saddr;
    uint16_t client_port = ntohs(udph->source);
    int idx = get_client_index(client_addr, client_port);
    if (idx == -1) continue;
    clients[idx].msg_count++;

    char reply[MSG_MAX_LEN + 32];
    if (msg_len == strlen(CLOSE_MSG) && strncmp(msg, CLOSE_MSG, msg_len) == 0) {
      for (int i = 0; i < client_count; ++i) {
        if (clients[i].addr.s_addr == client_addr.s_addr &&
            clients[i].port == client_port) {
          for (int j = i; j < client_count - 1; ++j) {
            clients[j] = clients[j + 1];
          }
          client_count--;
          printf("Client %s:%d disconnected and counter reset.\n",
                 inet_ntoa(client_addr), client_port);
          break;
        }
      }
      continue;
    } else {
      int reply_len = snprintf(reply, sizeof(reply), "%.*s [%d]", msg_len, msg,
                               clients[idx].msg_count);

      char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + reply_len];
      struct iphdr *iph_reply = (struct iphdr *)packet;
      struct udphdr *udph_reply =
          (struct udphdr *)(packet + sizeof(struct iphdr));
      char *data_reply = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
      memcpy(data_reply, reply, reply_len);

      iph_reply->ihl = 5;
      iph_reply->version = 4;
      iph_reply->tos = 0;
      iph_reply->tot_len =
          htons(sizeof(struct iphdr) + sizeof(struct udphdr) + reply_len);
      iph_reply->id = htons(rand() % 65535);
      iph_reply->frag_off = 0;
      iph_reply->ttl = 64;
      iph_reply->protocol = IPPROTO_UDP;
      iph_reply->check = 0;
      iph_reply->saddr = iph->daddr;
      iph_reply->daddr = iph->saddr;
      iph_reply->check =
          checksum((unsigned short *)iph_reply, sizeof(struct iphdr) / 2);

      udph_reply->source = udph->dest;
      udph_reply->dest = udph->source;
      udph_reply->len = htons(sizeof(struct udphdr) + reply_len);
      udph_reply->check = 0;

      memset(&daddr, 0, sizeof(daddr));
      daddr.sin_family = AF_INET;
      daddr.sin_addr.s_addr = iph->saddr;
      daddr.sin_port = udph->source;

      if (sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&daddr,
                 sizeof(daddr)) < 0) {
        perror("sendto");
      }
    }
  }
  close(sockfd);
  return 0;
}