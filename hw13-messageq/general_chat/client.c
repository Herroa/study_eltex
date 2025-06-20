#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_NAME 32
#define MAX_TEXT 192

struct chatmsg {
  long mtype;
  pid_t pid;
  char name[MAX_NAME];
  char text[MAX_TEXT];
  int event;
};

int q_in, q_out;
pid_t pid;
char name[MAX_NAME];
WINDOW *history_win, *input_win;
#define HISTORY_HEIGHT (LINES - 3)
#define INPUT_HEIGHT 3

void* receiver(void* arg) {
  struct chatmsg msg;
  while (1) {
    if (msgrcv(q_out, &msg, sizeof(msg) - sizeof(long), pid, 0) > 0) {
      if (msg.event == 0)
        wprintw(history_win, "%s: %s\n", msg.name, msg.text);
      else
        wprintw(history_win, "* %s *\n", msg.text);
      wrefresh(history_win);
      wmove(input_win, 1, 4);
      wrefresh(input_win);
    }
  }
  return NULL;
}

int main() {
  key_t key_in = ftok("server.c", 1);
  key_t key_out = ftok("server.c", 2);
  q_in = msgget(key_in, 0666);
  q_out = msgget(key_out, 0666);
  pid = getpid();
  printf("input name: ");
  fgets(name, MAX_NAME, stdin);
  name[strcspn(name, "\n")] = 0;
  struct chatmsg msg = {1, pid, "", "", 1};
  strncpy(msg.name, name, MAX_NAME);
  msgsnd(q_in, &msg, sizeof(msg) - sizeof(long), 0);

  initscr();
  cbreak();
  noecho();
  curs_set(1);
  int width = COLS;
  int height = LINES;
  history_win = newwin(height - 3, width, 0, 0);
  input_win = newwin(3, width, height - 3, 0);
  scrollok(history_win, TRUE);
  box(input_win, 0, 0);
  wrefresh(history_win);
  wrefresh(input_win);

  pthread_t recv_thread;
  pthread_create(&recv_thread, NULL, receiver, NULL);

  char text[MAX_TEXT];
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
      msg.event = 2;
      msgsnd(q_in, &msg, sizeof(msg) - sizeof(long), 0);
      break;
    }
    msg.event = 0;
    strncpy(msg.text, text, MAX_TEXT);
    msgsnd(q_in, &msg, sizeof(msg) - sizeof(long), 0);
    wmove(input_win, 1, 4);
    wrefresh(input_win);
  }
  endwin();
  return 0;
}
