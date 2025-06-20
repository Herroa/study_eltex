#define _XOPEN_SOURCE_EXTENDED
#include <curses.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILES 1000
#define MAX_PATH 1024

struct file_info {
  char name[256];
  char type;
  long size;
  time_t mtime;
};

struct file_info files[MAX_FILES];
int file_count = 0;
char current_path[MAX_PATH] = ".";
int selected_file = 0;
int start_display = 0;

void sig_winch(int signo) {
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
  resizeterm(size.ws_row, size.ws_col);
}

void load_directory(const char *path) {
  DIR *dir = opendir(path);
  if (!dir) {
    mvprintw(0, 0, "open dir error: %s", path);
    return;
  }

  file_count = 0;

  strcpy(files[file_count].name, ".");
  files[file_count].type = 'd';
  files[file_count].size = 0;
  files[file_count].mtime = time(NULL);
  file_count++;

  strcpy(files[file_count].name, "..");
  files[file_count].type = 'd';
  files[file_count].size = 0;
  files[file_count].mtime = time(NULL);
  file_count++;

  struct dirent *entry;
  while ((entry = readdir(dir)) && file_count < MAX_FILES) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    strncpy(files[file_count].name, entry->d_name, 255);
    files[file_count].name[255] = '\0';

    struct stat st;
    char full_path[MAX_PATH];
    snprintf(full_path, MAX_PATH, "%s/%s", path, entry->d_name);
    stat(full_path, &st);

    files[file_count].size = st.st_size;
    files[file_count].mtime = st.st_mtime;
    files[file_count].type = S_ISDIR(st.st_mode) ? 'd' : '-';

    file_count++;
  }
  closedir(dir);
}

void draw_interface() {
  clear();
  struct winsize size;
  ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

  attron(A_BOLD);
  mvprintw(0, 0, "Midnight Commander - %s", current_path);
  attroff(A_BOLD);

  int max_display = size.ws_row - 2;
  for (int i = 0; i < max_display && (i + start_display) < file_count; i++) {
    int idx = i + start_display;
    if (idx == selected_file) {
      attron(A_REVERSE);
    }

    char time_str[20];
    strftime(time_str, 20, "%Y-%m-%d %H:%M", localtime(&files[idx].mtime));

    mvprintw(i + 2, 0, "%c %-40s %10ld %s", files[idx].type, files[idx].name,
             files[idx].size, time_str);

    if (idx == selected_file) {
      attroff(A_REVERSE);
    }
  }

  mvprintw(size.ws_row - 1, 0,
           "KEY_Q KEY_UP KEY_DOWN KEY_PPAGE KEY_NPAGE KEY_BACKSPACE");
  refresh();
}

void exit_program() {
  endwin();
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  initscr();
  signal(SIGWINCH, sig_winch);
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  if (argc > 1) {
    strncpy(current_path, argv[1], MAX_PATH - 1);
  }

  load_directory(current_path);

  int ch;
  while ((ch = getch()) != KEY_F(1) && ch != 'q') {
    switch (ch) {
      case KEY_UP:
        if (selected_file > 0) selected_file--;
        break;
      case KEY_DOWN:
        if (selected_file < file_count - 1) selected_file++;
        break;
      case KEY_PPAGE:
        selected_file = (selected_file > 10) ? selected_file - 10 : 0;
        break;
      case KEY_NPAGE:
        selected_file = (selected_file + 10 < file_count) ? selected_file + 10
                                                          : file_count - 1;
        break;
      case '\n':
        if (files[selected_file].type == 'd') {
          char new_path[MAX_PATH];
          size_t current_len = strlen(current_path);
          size_t name_len = strlen(files[selected_file].name);

          if (current_len + name_len + 2 > MAX_PATH) {
            mvprintw(0, 0, "Path too long!");
            refresh();
            getch();
          } else {
            strcpy(new_path, current_path);
            strcat(new_path, "/");
            strcat(new_path, files[selected_file].name);

            strncpy(current_path, new_path, MAX_PATH - 1);
            current_path[MAX_PATH - 1] = '\0';
            load_directory(current_path);
            selected_file = 0;
          }
        }
        break;
      case KEY_BACKSPACE:
      case 127:
        char *last_slash = strrchr(current_path, '/');
        if (last_slash != NULL) {
          *last_slash = '\0';
          if (strlen(current_path) == 0) {
            strcpy(current_path, ".");
          }
          load_directory(current_path);
          selected_file = 0;
        }
        break;
    }

    draw_interface();
  }

  exit_program();
  return 0;
}
