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
  printf("Очередь создана, ID: %d\n", msgid);

  if (msgrcv(msgid, &msg, sizeof(msg.text), 1, 0) == -1) {
    perror("msgrcv");
    exit(1);
  }
  printf("Получено сообщение: %s\n", msg.text);

  return 0;
}