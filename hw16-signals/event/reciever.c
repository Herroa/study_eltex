#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

void sigusr1_handler(int sig, siginfo_t *info, void *context) {
  printf("Получен сигнал SIGUSR1\n");
  printf("От процесса %d\n", info->si_pid);
}

int main() {
  sigset_t mask;
  int signo;

  printf("PID: %d\n", getpid());

  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    perror("sigprocmask");
    return 1;
  }

  printf("Ожидание сигнала SIGUSR1...\n");

  while (1) {
    if (sigwait(&mask, &signo) != 0) {
      perror("sigwait");
      continue;
    }

    printf("Получен сигнал SIGUSR1\n");
  }

  return 0;
}
