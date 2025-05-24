#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Использование: %s <PID>\n", argv[0]);
    return 1;
  }

  printf("PID: %d\n", getpid());

  pid_t target_pid = atoi(argv[1]);

  if (kill(target_pid, SIGINT) == -1) {
    perror("kill");
    return 1;
  }

  printf("Сигнал SIGINT отправлен процессу %d\n", target_pid);
  return 0;
}