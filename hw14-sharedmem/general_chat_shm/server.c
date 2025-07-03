#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 32
#define MAX_NAME 32
#define MAX_TEXT 192
#define SHM_NAME "/my_shared_memory"
#define SEM_MUTEX_NAME "/sem_mutex"
#define SEM_MSG_NAME "/sem_msg"
#define SHM_SIZE 65536
#define MAX_MSGS 256

struct chatmsg {
  pid_t pid;
  char name[MAX_NAME];
  char text[MAX_TEXT];
  int event;
};

struct client {
  pid_t pid;
  char name[MAX_NAME];
  int active;
  int last_msg_idx;
};

struct shared_chat {
  struct chatmsg messages[MAX_MSGS];
  int msg_write_idx;
  struct client clients[MAX_CLIENTS];
  int nclients;
};

int main() {
  int shm_fd;
  struct shared_chat *chat;
  sem_t *sem_mutex, *sem_msg;

  shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(1);
  }
  if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    perror("ftruncate");
    exit(1);
  }
  chat = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (chat == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0666, 1);
  sem_msg = sem_open(SEM_MSG_NAME, O_CREAT, 0666, 0);
  if (sem_mutex == SEM_FAILED || sem_msg == SEM_FAILED) {
    perror("sem_open");
    exit(1);
  }

  sem_wait(sem_mutex);
  chat->msg_write_idx = 0;
  chat->nclients = 0;
  for (int i = 0; i < MAX_CLIENTS; ++i) chat->clients[i].active = 0;
  sem_post(sem_mutex);

  printf("Server started...\n");
  while (1) {
    sem_wait(sem_msg);
    sem_wait(sem_mutex);
    int idx = chat->msg_write_idx - 1;
    if (idx < 0) idx += MAX_MSGS;
    struct chatmsg *msg = &chat->messages[idx];
    if (msg->event == 1) {
      int found = 0;
      for (int i = 0; i < chat->nclients; ++i) {
        if (chat->clients[i].pid == msg->pid && chat->clients[i].active)
          found = 1;
      }
      if (!found && chat->nclients < MAX_CLIENTS) {
        chat->clients[chat->nclients].pid = msg->pid;
        strncpy(chat->clients[chat->nclients].name, msg->name, MAX_NAME);
        chat->clients[chat->nclients].active = 1;
        chat->clients[chat->nclients].last_msg_idx = chat->msg_write_idx;
        chat->nclients++;
        snprintf(msg->text, MAX_TEXT, "%s joined", msg->name);
      }
    } else if (msg->event == 2) {
      for (int i = 0; i < chat->nclients; ++i) {
        if (chat->clients[i].pid == msg->pid && chat->clients[i].active) {
          snprintf(msg->text, MAX_TEXT, "%s left", chat->clients[i].name);
          chat->clients[i].active = 0;
        }
      }
    }
    sem_post(sem_mutex);
  }
  munmap(chat, SHM_SIZE);
  close(shm_fd);
  shm_unlink(SHM_NAME);
  sem_close(sem_mutex);
  sem_close(sem_msg);
  sem_unlink(SEM_MUTEX_NAME);
  sem_unlink(SEM_MSG_NAME);
  return 0;
}
