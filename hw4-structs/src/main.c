#include "catalog.h"
#include <stdio.h>

int main() {
  int n = 0;
  Abonent catalog[MAX_ABONENTS];

  int choice;
  do {
    printf("Меню:\n1) Добавить абонента\n2) Удалить абонента\n3) Поиск "
           "абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
    scanf("%d", &choice);
    switch (choice) {
    case 1:
      add_catalog(catalog, &n);
      break;
    case 2:
      remove_catalog(catalog, n);
      break;
    case 3:
      search_catalog(catalog, n);
      break;
    case 4:
      print_catalog(catalog, n);
      break;
    case 5:
      printf("Завершение программы.\n");
      break;
    }
  } while (choice != 5);

  return 0;
}
