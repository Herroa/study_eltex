
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 32
#define MAX_NAME 32
#define MAX_TEXT 192

struct chatmsg {
  long mtype;
  pid_t pid;
  char name[MAX_NAME];
  char text[MAX_TEXT];
  int event;
};

struct client {
  pid_t pid;
  char name[MAX_NAME];
};

int main() {
  key_t key_in = ftok("server.c", 1);
  key_t key_out = ftok("server.c", 2);
  int tmpid;
  if ((tmpid = msgget(key_in, 0666)) != -1) msgctl(tmpid, IPC_RMID, NULL);
  if ((tmpid = msgget(key_out, 0666)) != -1) msgctl(tmpid, IPC_RMID, NULL);
  int q_in = msgget(key_in, 0666 | IPC_CREAT);
  int q_out = msgget(key_out, 0666 | IPC_CREAT);
  struct chatmsg msg;
  struct client clients[MAX_CLIENTS];
  int nclients = 0;
  while (1) {
    if (msgrcv(q_in, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1) continue;
    if (msg.event == 1) {
      if (nclients < MAX_CLIENTS) {
        clients[nclients].pid = msg.pid;
        strncpy(clients[nclients].name, msg.name, MAX_NAME);
        nclients++;
      }
      snprintf(msg.text, MAX_TEXT, "%s joined", msg.name);
    } else if (msg.event == 2) {
      int i;
      for (i = 0; i < nclients; ++i) {
        if (clients[i].pid == msg.pid) {
          snprintf(msg.text, MAX_TEXT, "%s left", clients[i].name);
          for (int j = i; j < nclients - 1; ++j) clients[j] = clients[j + 1];
          nclients--;
          break;
        }
      }
    }
    for (int i = 0; i < nclients; ++i) {
      msg.mtype = clients[i].pid;
      msgsnd(q_out, &msg, sizeof(msg) - sizeof(long), 0);
    }
  }
  msgctl(q_in, IPC_RMID, NULL);
  msgctl(q_out, IPC_RMID, NULL);
  return 0;
}
