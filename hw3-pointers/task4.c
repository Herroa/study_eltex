#include <stdio.h>

int my_strlen(char *str) {
  int len = 0;
  while (str[len] != '\0') {
    len++;
  }
  return len;
}

char *my_strstr(char *str, char *sub) {
  int str_len = my_strlen(str);
  int sub_len = my_strlen(sub);
  if (str_len < sub_len)
    return NULL;
  for (int i = 0; i < str_len; i++) {
    int flag = 1;
    for (int j = 0; j < sub_len; j++) {
      if (str[i + j] != sub[j]) {
        flag = 0;
        break;
      }
    }
    if (flag)
      return &str[i];
  }
  return NULL;
}

int main() {
  char string[100];
  char substring[100];
  printf("Input str: \n");
  scanf("%99s", string);
  printf("Input sub: \n");
  scanf("%99s", substring);
  char *result = my_strstr(string, substring);
  if (result != NULL) {
    printf("Found.: %s\n", result);
  } else {
    printf("Not found.\n");
  }
  return 0;
}