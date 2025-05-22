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
  char buffer[100];

  fd = open(FIFO_NAME, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  if (read(fd, buffer, sizeof(buffer)) == -1) {
    perror("read");
    return 1;
  }

  printf("recv: %s\n", buffer);

  close(fd);

  return 0;
}
