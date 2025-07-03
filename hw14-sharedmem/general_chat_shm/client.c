#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
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

struct client_info {
  int idx;
  int active;
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

struct updater_ctx {
  struct shared_chat* chat;
  int my_idx;
  WINDOW* history_win;
  sem_t* sem_mutex;
};

void* history_updater(void* arg) {
  struct updater_ctx* ctx = arg;
  int last_idx = ctx->chat->clients[ctx->my_idx].last_msg_idx;
  while (1) {
    sem_wait(ctx->sem_mutex);
    int cur_idx = ctx->chat->msg_write_idx;
    while (last_idx != cur_idx) {
      int midx = last_idx % MAX_MSGS;
      struct chatmsg* msg = &ctx->chat->messages[midx];
      if (msg->event == 0)
        wprintw(ctx->history_win, "%s: %s\n", msg->name, msg->text);
      else
        wprintw(ctx->history_win, "* %s *\n", msg->text);
      wrefresh(ctx->history_win);
      last_idx = (last_idx + 1) % MAX_MSGS;
    }
    ctx->chat->clients[ctx->my_idx].last_msg_idx = last_idx;
    sem_post(ctx->sem_mutex);
    usleep(100 * 1000);
  }
  return NULL;
}

int main() {
  int shm_fd;
  struct shared_chat* chat;
  sem_t *sem_mutex, *sem_msg;
  pid_t pid = getpid();
  char name[MAX_NAME];
  printf("input name: ");
  fgets(name, MAX_NAME, stdin);
  name[strcspn(name, "\n")] = 0;

  shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(1);
  }
  chat = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (chat == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  sem_mutex = sem_open(SEM_MUTEX_NAME, 0);
  sem_msg = sem_open(SEM_MSG_NAME, 0);
  if (sem_mutex == SEM_FAILED || sem_msg == SEM_FAILED) {
    perror("sem_open");
    exit(1);
  }

  sem_wait(sem_mutex);
  int my_idx = -1;
  for (int i = 0; i < MAX_CLIENTS; ++i) {
    if (!chat->clients[i].active) {
      chat->clients[i].pid = pid;
      strncpy(chat->clients[i].name, name, MAX_NAME);
      chat->clients[i].active = 1;
      chat->clients[i].last_msg_idx = chat->msg_write_idx;
      my_idx = i;
      break;
    }
  }
  int idx = chat->msg_write_idx % MAX_MSGS;
  chat->messages[idx].pid = pid;
  strncpy(chat->messages[idx].name, name, MAX_NAME);
  chat->messages[idx].event = 1;
  chat->msg_write_idx = (chat->msg_write_idx + 1) % MAX_MSGS;
  sem_post(sem_msg);
  sem_post(sem_mutex);

  if (my_idx == -1) {
    printf("No space for new clients!\n");
    exit(1);
  }

  initscr();
  cbreak();
  noecho();
  curs_set(1);
  int width = COLS;
  int height = LINES;
  WINDOW* history_win = newwin(height - 3, width, 0, 0);
  WINDOW* input_win = newwin(3, width, height - 3, 0);
  scrollok(history_win, TRUE);
  box(input_win, 0, 0);
  wrefresh(history_win);
  wrefresh(input_win);

  char text[MAX_TEXT];
  pthread_t updater_thread;
  struct updater_ctx ctx = {chat, my_idx, history_win, sem_mutex};
  pthread_create(&updater_thread, NULL, history_updater, &ctx);

  while (1) {
    werase(input_win);
    box(input_win, 0, 0);
    mvwprintw(input_win, 1, 2, "> ");
    wmove(input_win, 1, 4);
    wrefresh(input_win);
    echo();
    wgetnstr(input_win, text, MAX_TEXT - 1);
    noecho();
    if (strcmp(text, "exit") == 0) {
      sem_wait(sem_mutex);
      int idx = chat->msg_write_idx % MAX_MSGS;
      chat->messages[idx].pid = pid;
      strncpy(chat->messages[idx].name, name, MAX_NAME);
      chat->messages[idx].event = 2;
      chat->msg_write_idx = (chat->msg_write_idx + 1) % MAX_MSGS;
      chat->clients[my_idx].active = 0;
      sem_post(sem_msg);
      sem_post(sem_mutex);
      break;
    }
    sem_wait(sem_mutex);
    int idx2 = chat->msg_write_idx % MAX_MSGS;
    chat->messages[idx2].pid = pid;
    strncpy(chat->messages[idx2].name, name, MAX_NAME);
    strncpy(chat->messages[idx2].text, text, MAX_TEXT);
    chat->messages[idx2].event = 0;
    chat->msg_write_idx = (chat->msg_write_idx + 1) % MAX_MSGS;
    sem_post(sem_msg);
    sem_post(sem_mutex);
    wmove(input_win, 1, 4);
    wrefresh(input_win);
  }
  endwin();
  munmap(chat, SHM_SIZE);
  close(shm_fd);
  sem_close(sem_mutex);
  sem_close(sem_msg);
  return 0;
}
