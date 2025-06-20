#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

struct message {
  long mtype;
  char text[100];
};

int main() {
  key_t key;
  int msgid;
  struct message msg;

  strcpy(msg.text, "Hi!");
  msg.mtype = 1;

  key = ftok("/tmp", 'A');
  if (key == -1) {
    perror("ftok");
    exit(1);
  }

  msgid = msgget(key, 0666 | IPC_CREAT);
  if (msgid == -1) {
    perror("msgid");
    exit(1);
  }

  if (msgsnd(msgid, &msg, strlen(msg.text) + 1, 0) == -1) {
    perror("msgsnd");
    exit(1);
  }

  printf("Сообщение отправлено\n");
  return 0;
}