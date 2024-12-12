#include "catalog.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void create_catalog(Abonent **catalog, int *n) {
  *catalog = malloc(sizeof(Abonent));
  if (*catalog != NULL) {
    printf("Введите имя: \n");
    scanf("%9s", (*catalog)[0].name);
    printf("Введите фамилию: \n");
    scanf("%9s", (*catalog)[0].second_name);
    printf("Введите телефон: \n");
    scanf("%9s", (*catalog)[0].tel);
    printf("Абонент %s добавлен.\n", (*catalog)[0].name);
    *n = 1;
  } else {
    printf("Ошибка выделения памяти.\n");
  }
}

void add_catalog(Abonent **catalog, int *n) {
  Abonent *tmp = realloc(*catalog, sizeof(Abonent) * (*n + 1));
  if (tmp != NULL) {
    *catalog = tmp;
    printf("Введите имя: \n");
    scanf("%9s", (*catalog)[*n].name);
    printf("Введите фамилию: \n");
    scanf("%9s", (*catalog)[*n].second_name);
    printf("Введите телефон: \n");
    scanf("%9s", (*catalog)[*n].tel);
    printf("Абонент %s добавлен.\n", (*catalog)[*n].name);
    (*n)++;
  } else {
    printf("Ошибка перераспределения памяти.\n");
  }
}

void search_catalog(Abonent *catalog, int n) {
  char string[16];
  printf("Введите имя для поиска: \n");
  scanf("%9s", string);
  int found = 0;
  for (int i = 0; i < n; i++) {
    if (strcmp(string, catalog[i].name) == 0) {
      printf("Имя: %s, Фамилия: %s, Телефон: %s\n", catalog[i].name,
             catalog[i].second_name, catalog[i].tel);
      found = 1;
    }
  }
  if (!found) {
    printf("Абоненты с таким именем не найдены.\n");
  }
}

void remove_catalog(Abonent **catalog, int *n) {
  char name[10];
  printf("Введите имя для удаления: \n");
  scanf("%9s", name);

  int found = 0;
  for (int i = 0; i < *n; i++) {
    if (strcmp((*catalog)[i].name, name) == 0) {
      found = 1;
      for (int j = i; j < *n - 1; j++) {
        (*catalog)[j] = (*catalog)[j + 1];
      }
      Abonent *tmp = realloc(*catalog, sizeof(Abonent) * (*n - 1));
      if (tmp != NULL || *n == 1) {
        *catalog = tmp;
        (*n)--;
        printf("Абонент %s удален.\n", name);
      } else {
        printf("Ошибка при уменьшении размера каталога.\n");
      }
      return;
    }
  }
  if (!found) {
    printf("Абонент с именем %s не найден.\n", name);
  }
}

void print_catalog(Abonent *catalog, int n) {
  printf("Весь каталог: \n");
  for (int i = 0; i < n; i++) {
    if (strlen(catalog[i].name) > 0) {
      printf("Имя: %s, Фамилия: %s, Телефон: %s\n", catalog[i].name,
             catalog[i].second_name, catalog[i].tel);
    }
  }
}