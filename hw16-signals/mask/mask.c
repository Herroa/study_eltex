#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void sigint_handler(int sig) { printf("Получен SIGINT\n"); }

int main() {
  printf("PID: %d\n", getpid());
  sigset_t mask;

  signal(SIGINT, sigint_handler);

  sigemptyset(&mask);

  sigaddset(&mask, SIGINT);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    perror("sigprocmask");
    return 1;
  }

  while (1) {
    pause();
  }

  return 0;
}