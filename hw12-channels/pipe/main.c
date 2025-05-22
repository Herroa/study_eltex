#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int fd[2];
  pid_t pid;
  char message[100] = "Hi!";
  char buffer[100];

  if (pipe(fd) == -1) {
    return 1;
  }

  pid = fork();

  if (pid == -1) {
    return 1;
  }

  if (pid == 0) {
    close(fd[1]);
    read(fd[0], buffer, sizeof(buffer));
    printf("recv: %s\n", buffer);
    close(fd[0]);
  } else {
    close(fd[0]);
    write(fd[1], message, sizeof(message));
    close(fd[1]);
    wait(NULL);
  }

  return 0;
}