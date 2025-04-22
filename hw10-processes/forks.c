#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void print_process_info(const char *process_name) {
  printf("%s: PID = %d, PPID = %d\n", process_name, getpid(), getppid());
}

int main() {
  pid_t pid1 = fork();

  if (pid1 < 0) {
    perror("Ошибка при создании процесса1");
    exit(EXIT_FAILURE);
  } else if (pid1 == 0) {
    print_process_info("Процесс1");

    pid_t pid3 = fork();
    if (pid3 < 0) {
      perror("Ошибка при создании процесса3");
      exit(EXIT_FAILURE);
    } else if (pid3 == 0) {
      print_process_info("Процесс3");
      exit(0);
    }

    pid_t pid4 = fork();
    if (pid4 < 0) {
      perror("Ошибка при создании процесса4");
      exit(EXIT_FAILURE);
    } else if (pid4 == 0) {
      print_process_info("Процесс4");
      exit(0);
    }

    wait(NULL);
    wait(NULL);

    exit(0);
  }

  pid_t pid2 = fork();

  if (pid2 < 0) {
    perror("Ошибка при создании процесса2");
    exit(EXIT_FAILURE);
  } else if (pid2 == 0) {
    print_process_info("Процесс2");

    pid_t pid5 = fork();
    if (pid5 < 0) {
      perror("Ошибка при создании процесса5");
      exit(EXIT_FAILURE);
    } else if (pid5 == 0) {
      print_process_info("Процесс5");
      exit(0);
    }

    wait(NULL);

    exit(0);
  }

  wait(NULL);
  wait(NULL);

  return 0;
}