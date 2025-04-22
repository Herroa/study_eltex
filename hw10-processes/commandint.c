#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define SIZE 1024

void parse_input(char *input, char **args) {
  int i = 0;
  args[i] = strtok(input, " \n");
  while (args[i] != NULL) {
    args[++i] = strtok(NULL, " \n");
  }
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
    parse_input(input, args);
    if (args[0] == NULL) continue;

    pid_t pid = fork();
    if (pid < 0) {
      perror("Ошибка при создании процесса\n");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      if (execvp(args[0], args) < 0) {
        perror("Ошибка выполнения файла\n");
        exit(EXIT_FAILURE);
      }
    } else {
      wait(NULL);
    }
  }

  printf("Выход из интерпретатора\n");
  return 0;
}