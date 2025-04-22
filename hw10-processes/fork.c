#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid = fork();

  if (pid < 0) {
    perror("Ошибка при создании процесса");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
    int exit_status = 1;
    printf("Дочерний процесс завершается со статусом %d\n", exit_status);
    exit(exit_status);
  } else if (pid > 0) {
    printf("Родительский процесс: PID = %d, PPID = %d\n", getpid(), getppid());
    int status;
    wait(&status);

    if (WIFEXITED(status)) {
      printf("Дочерний процесс завершился со статусом %d\n",
             WEXITSTATUS(status));
    } else {
      printf("Дочерний процесс завершился нештатно\n");
    }
  }

  return 0;
}