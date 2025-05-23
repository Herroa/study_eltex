#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 1024
#define MAX_ARGS 10

void parse_input(char *input, char **args) {
  int i = 0;
  args[i] = strtok(input, " \n");
  while (args[i] != NULL) {
    args[++i] = strtok(NULL, " \n");
  }
}

void execute_pipeline(char *cmd1, char *cmd2) {
  int pipefd[2];
  pid_t pid1, pid2;
  char *args1[MAX_ARGS];
  char *args2[MAX_ARGS];

  if (pipe(pipefd) == -1) {
    perror("pipe");
    return;
  }

  parse_input(cmd1, args1);
  parse_input(cmd2, args2);

  pid1 = fork();
  if (pid1 == 0) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    if (execvp(args1[0], args1) == -1) {
      perror("execvp");
      exit(1);
    }
  }

  pid2 = fork();
  if (pid2 == 0) {
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);

    if (execvp(args2[0], args2) == -1) {
      perror("execvp");
      exit(1);
    }
  }

  close(pipefd[0]);
  close(pipefd[1]);

  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
}

int main() {
  char input[SIZE];
  char *args[SIZE];

  while (1) {
    printf("> ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
      printf("\n");
      break;
    }
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "exit") == 0) break;

    char *pipe_pos = strstr(input, "|");
    if (pipe_pos != NULL) {
      *pipe_pos = '\0';
      char *cmd1 = input;
      char *cmd2 = pipe_pos + 1;
      while (*cmd2 == ' ') cmd2++;
      execute_pipeline(cmd1, cmd2);
    } else {
      parse_input(input, args);
      if (args[0] == NULL) continue;

      pid_t pid = fork();
      if (pid == 0) {
        if (execvp(args[0], args) == -1) {
          perror("execvp");
          exit(1);
        }
      } else {
        wait(NULL);
      }
    }
  }

  printf("Выход из интерпретатора\n");
  return 0;
}