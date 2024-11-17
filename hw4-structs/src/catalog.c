#include "catalog.h"
#include "stdio.h"
#include "string.h"

void add_catalog(Abonent catalog[], int *n) {
  if (*n >= MAX_ABONENTS) {
    printf("Справочник переполнен. Добавление невозможно.\n");
    return;
  }
  printf("Введите имя: \n");
  scanf("%9s", catalog[*n].name);
  printf("Введите фамилию: \n");
  scanf("%9s", catalog[*n].second_name);
  printf("Введите телефон: \n");
  scanf("%9s", catalog[*n].tel);
  printf("Абонент %s добавлен.\n", catalog[*n].name);
  (*n)++;
}

void search_catalog(Abonent catalog[], int n) {
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

void remove_catalog(Abonent catalog[], int n) {
  char name[10];
  printf("Введите имя для удаления: \n");
  scanf("%9s", name);
  for (int i = 0; i < n; i++) {
    if (strcmp(catalog[i].name, name) == 0) {
      strcpy(catalog[i].name, "");
      strcpy(catalog[i].second_name, "");
      strcpy(catalog[i].tel, "");
      printf("Абонент %s удален.\n", catalog[i].name);
      return;
    }
  }
  printf("Абонент не найден.\n");
}

void print_catalog(Abonent catalog[], int n) {
  printf("Весь каталог: \n");
  for (int i = 0; i < n; i++) {
    if (strlen(catalog[i].name) > 0) {
      printf("Имя: %s, Фамилия: %s, Телефон: %s\n", catalog[i].name,
             catalog[i].second_name, catalog[i].tel);
    }
  }
}