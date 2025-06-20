#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_TEXT 256
#define MAX_NAME 32

struct msgbuf {
  long mtype;
  char mtext[MAX_TEXT];
};

int main() {
  key_t key = ftok("server.c", 65);
  if (key == -1) {
    perror("ftok");
    exit(1);
  }

  int msgid = msgget(key, 0666);
  if (msgid == -1) {
    perror("msgget");
    exit(1);
  }

  struct msgbuf message;
  char name[MAX_NAME];
  char text[MAX_TEXT - MAX_NAME];

  printf("Your name: ");
  if (fgets(name, MAX_NAME, stdin) == NULL) {
    printf("error input\n");
    exit(1);
  }
  name[strcspn(name, "\n")] = '\0';

  while (1) {
    printf("input(exit for leave): ");
    if (fgets(text, sizeof(text), stdin) == NULL) {
      printf("error input\n");
      exit(1);
    }
    text[strcspn(text, "\n")] = '\0';

    snprintf(message.mtext, MAX_TEXT, "%s: %s", name, text);

    if (msgsnd(msgid, &message, strlen(message.mtext) + 1, 0) == -1) {
      perror("msgsnd");
      exit(1);
    }

    if (strncmp(text, "exit", 4) == 0) {
      break;
    }
  }

  return 0;
}
