#define _POSIX_C_SOURCE 199309L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sigusr1_handler(int sig, siginfo_t *info, void *context) {
  printf("Получен сигнал SIGUSR1\n");
  printf("От процесса %d\n", info->si_pid);
}

int main() {
  printf("PID: %d\n", getpid());
  struct sigaction sa;

  memset(&sa, 0, sizeof(sa));

  sa.sa_sigaction = sigusr1_handler;
  sa.sa_flags = SA_SIGINFO;

  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    return 1;
  }

  while (1) {
    pause();
  }
  return 0;
}