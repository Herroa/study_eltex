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

  mq = mq_open(QUEUE_NAME, O_RDONLY);
  check_error(mq, "mq_open");

  printf("Получение сообщений:\n");
  while (1) {
    ssize_t bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
    check_error(bytes_read, "mq_receive");

    printf("Получено: %s\n", buffer);

    if (strcmp(buffer, MSG_STOP) == 0) {
      break;
    }
  }

  status = mq_close(mq);
  check_error(status, "mq_close");

  status = mq_unlink(QUEUE_NAME);
  check_error(status, "mq_unlink");

  return 0;
}