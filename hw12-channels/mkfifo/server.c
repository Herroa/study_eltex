#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/server_fifo"

int main() {
  int fd;
  char message[100] = "Hi!";

  unlink(FIFO_NAME);

  if (mkfifo(FIFO_NAME, 0666) == -1) {
    perror("mkfifo");
    return 1;
  }

  fd = open(FIFO_NAME, O_WRONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  if (write(fd, message, strlen(message) + 1) == -1) {
    perror("write");
    return 1;
  }

  printf("send: %s\n", message);

  close(fd);

  unlink(FIFO_NAME);

  return 0;
}
