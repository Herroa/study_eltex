#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 256

struct msgbuf {
  long mtype;
  char mtext[MAX_TEXT];
};

int main() {
  key_t key = ftok("server.c", 65);
  int msgid = msgget(key, 0666 | IPC_CREAT);

  if (msgid == -1) {
    perror("msgget");
    exit(1);
  }

  struct msgbuf message;

  printf("server up\n");

  while (1) {
    if (msgrcv(msgid, &message, sizeof(message.mtext), 0, 0) == -1) {
      perror("msgrcv");
      exit(1);
    }
    char *colon = strstr(message.mtext, ": ");
    if (colon) {
      *colon = '\0';
      char *name = message.mtext;
      char *text = colon + 2;
      printf("%s >> %s\n", name, text);
    } else {
      printf("%s\n", message.mtext);
    }

    if (strncmp(message.mtext, "exit", 4) == 0) {
      printf("server down\n");
      break;
    }
  }

  msgctl(msgid, IPC_RMID, NULL);

  return 0;
}
