// Задание: fork + poll/epoll/select + timers + IPC

// Управление такси.

// Есть опорный пункт - исходное приложение. Запускается один раз и привязано к
// терминалу, имеет CLI.

// В CLI должны быть команды create_driver, send_task <pid> <task_timer>,
// get_status <pid>, get_drivers.

// create_driver создает новый процесс (driver), который ожидает команд от CLI,
// pid процесса будет использоваться  для обращения к этому driver.

// send_task <pid> <task_timer> создаст задачу для driver номер <pid> и займет
// его на <task_timer> секунд. Если попытаться обратиться к этому driver во
// время выполнения задания с помощью send_task, то driver должен послать ошибку
// Busy <task_timer>. По истечению <task_timer> секунд driver меняет свое
// состояние на Available.

// get_status <pid> показывает статус driver с номером <pid>. Может быть Busy
// <task_timer> либо Available.

// get_drivers показывает статусы и pid всех drivers запущенных из этого
// инстанса CLI.

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX 32
#define FIFO_LEN 64

int pids[MAX], fds[MAX], state[MAX], busy_until[MAX], n = 0;

void invite() {
  printf("> ");
  fflush(stdout);
}

int main() {
  char cmd[64];
  printf(
      "help: create_driver, send_task <pid> <seconds>, get_status <pid>, "
      "get_drivers, exit.\n");
  invite();

  int epfd = epoll_create1(0);
  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.u32 = 0;
  epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev);

  while (1) {
    struct epoll_event evs[MAX];
    int nfds = epoll_wait(epfd, evs, MAX, 500);
    for (int k = 0; k < nfds; k++) {
      int fd = evs[k].data.u32;
      if (fd == 0) {
        if (!fgets(cmd, 64, stdin)) continue;
        if (!strncmp(cmd, "create_driver", 13) && n < MAX) {
          pid_t pid = fork();
          if (pid == 0) {
            char fifo[FIFO_LEN];
            snprintf(fifo, FIFO_LEN, "/tmp/driver_%d.fifo", getpid());
            mkfifo(fifo, 0666);
            int busy = 0, until = 0;
            while (1) {
              int fd = open(fifo, O_RDONLY);
              char buf[64];
              int m = read(fd, buf, 63);
              if (m > 0) {
                buf[m] = 0;
                if (!strncmp(buf, "send_task", 9)) {
                  int t = 0;
                  sscanf(buf + 9, "%d", &t);
                  if (busy && until > time(0))
                    printf("Driver %d Busy %ld\n", getpid(),
                           (long)(until - time(0)));
                  else {
                    busy = 1;
                    until = time(0) + t;
                    printf("Driver %d Start %d\n", getpid(), t);
                    invite();
                  }
                } else if (!strncmp(buf, "get_status", 10)) {
                  if (busy && until > time(0))
                    printf("Driver %d Busy %ld\n", getpid(),
                           (long)(until - time(0)));
                  else
                    printf("Driver %d Available\n", getpid());
                  invite();
                }
              }
              close(fd);
              if (busy && until <= time(0)) {
                busy = 0;
                until = 0;
                printf("Driver %d Available\n", getpid());
                invite();
              }
            }
            exit(0);
          } else if (pid > 0) {
            char fifo[FIFO_LEN];
            snprintf(fifo, FIFO_LEN, "/tmp/driver_%d.fifo", pid);
            mkfifo(fifo, 0666);
            pids[n] = pid;
            fds[n] = open(fifo, O_RDONLY | O_NONBLOCK);
            state[n] = 0;
            busy_until[n] = 0;
            struct epoll_event ev2 = {.events = EPOLLIN, .data.u32 = pid};
            epoll_ctl(epfd, EPOLL_CTL_ADD, fds[n], &ev2);
            printf("Driver PID: %d\n", pid);
            invite();
            n++;
          }
        } else if (!strncmp(cmd, "send_task", 9)) {
          int pid, t;
          if (sscanf(cmd, "send_task %d %d", &pid, &t) == 2)
            for (int i = 0; i < n; i++)
              if (pids[i] == pid) {
                char fifo[FIFO_LEN], buf[32];
                snprintf(fifo, FIFO_LEN, "/tmp/driver_%d.fifo", pid);
                int fdw = open(fifo, O_WRONLY | O_NONBLOCK);
                snprintf(buf, 32, "send_task %d", t);
                write(fdw, buf, strlen(buf));
                close(fdw);
              }
        } else if (!strncmp(cmd, "get_status", 10)) {
          int pid;
          if (sscanf(cmd, "get_status %d", &pid) == 1)
            for (int i = 0; i < n; i++)
              if (pids[i] == pid) {
                if (state[i])
                  printf("Driver %d: Busy %ld\n", pid,
                         (long)(busy_until[i] - time(0)));
                else
                  printf("Driver %d: Available\n", pid);
                invite();
              }
        } else if (!strncmp(cmd, "get_drivers", 11)) {
          for (int i = 0; i < n; i++) {
            if (state[i])
              printf("%d: Busy %ld\n", pids[i],
                     (long)(busy_until[i] - time(0)));
            else
              printf("%d: Available\n", pids[i]);
          }
          invite();
        } else if (!strncmp(cmd, "exit", 4))
          return 0;
        else {
          printf(
              "help: create_driver, send_task <pid> <seconds>, get_status "
              "<pid>, get_drivers, exit.\n");
          invite();
        }
      } else {
        for (int i = 0; i < n; i++)
          if (pids[i] == fd) {
            char buf[64];
            int m = read(fds[i], buf, 63);
            if (m > 0) {
              buf[m] = 0;
              if (!strncmp(buf, "send_task", 9)) {
                int t = 0;
                sscanf(buf + 9, "%d", &t);
                if (state[i] && busy_until[i] > time(0))
                  printf("Driver %d Busy %ld\n", fd,
                         (long)(busy_until[i] - time(0)));
                else {
                  state[i] = 1;
                  busy_until[i] = time(0) + t;
                  printf("Driver %d Start %d\n", fd, t);
                  invite();
                }
              } else if (!strncmp(buf, "get_status", 10)) {
                if (state[i] && busy_until[i] > time(0))
                  printf("Driver %d Busy %ld\n", fd,
                         (long)(busy_until[i] - time(0)));
                else {
                  printf("Driver %d Available\n", fd);
                  invite();
                }
              }
            }
          }
      }
    }
    for (int i = 0; i < n; i++)
      if (state[i] && busy_until[i] <= time(0)) {
        state[i] = 0;
        busy_until[i] = 0;
        printf("Driver %d Available\n", pids[i]);
        invite();
      }
  }
  for (int i = 0; i < n; i++) {
    close(fds[i]);
    char fifo[FIFO_LEN];
    snprintf(fifo, FIFO_LEN, "/tmp/driver_%d.fifo", pids[i]);
    unlink(fifo);
  }
  close(epfd);
  return 0;
}
