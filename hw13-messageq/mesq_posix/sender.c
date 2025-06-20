#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define QUEUE_NAME "/my_queue"
#define MAX_SIZE 1024
#define MSG_STOP "exit"

void check_error(int status, const char *msg) {
  if (status == -1) {
    perror(msg);
    exit(1);
  }
}

int main() {
  mqd_t mq;
  struct mq_attr attr;
  char buffer[MAX_SIZE];
  int status;

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_SIZE;
  attr.mq_curmsgs = 0;

  mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
  check_error(mq, "mq_open");

  printf("Input ('exit' for quit)):\n");
  while (1) {
    printf("> ");
    fgets(buffer, MAX_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    status = mq_send(mq, buffer, strlen(buffer) + 1, 0);
    check_error(status, "mq_send");

    if (strcmp(buffer, MSG_STOP) == 0) {
      break;
    }
  }

  status = mq_close(mq);
  check_error(status, "mq_close");

  return 0;
}