#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_NAME 32
#define MAX_TEXT 192

struct chatmsg {
  long mtype;
  pid_t pid;
  char name[MAX_NAME];
  char text[MAX_TEXT];
  int event;
};

int q_in, q_out;
pid_t pid;
char name[MAX_NAME];

void* receiver(void* arg) {
  struct chatmsg msg;
  while (1) {
    if (msgrcv(q_out, &msg, sizeof(msg) - sizeof(long), pid, 0) > 0) {
      if (msg.event == 0)
        printf("%s: %s\n", msg.name, msg.text);
      else
        printf("* %s *\n", msg.text);
    }
  }
  return NULL;
}

int main() {
  key_t key_in = ftok("server.c", 1);
  key_t key_out = ftok("server.c", 2);
  q_in = msgget(key_in, 0666);
  q_out = msgget(key_out, 0666);
  pid = getpid();
  printf("input name: ");
  fgets(name, MAX_NAME, stdin);
  name[strcspn(name, "\n")] = 0;
  struct chatmsg msg = {1, pid, "", "", 1};
  strncpy(msg.name, name, MAX_NAME);
  msgsnd(q_in, &msg, sizeof(msg) - sizeof(long), 0);
  pthread_t recv_thread;
  pthread_create(&recv_thread, NULL, receiver, NULL);
  while (1) {
    char text[MAX_TEXT];
    fgets(text, MAX_TEXT, stdin);
    text[strcspn(text, "\n")] = 0;
    if (strcmp(text, "exit") == 0) {
      msg.event = 2;
      msgsnd(q_in, &msg, sizeof(msg) - sizeof(long), 0);
      break;
    }
    msg.event = 0;
    strncpy(msg.text, text, MAX_TEXT);
    msgsnd(q_in, &msg, sizeof(msg) - sizeof(long), 0);
  }
  return 0;
}
