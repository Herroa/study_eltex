#include "catalog.h"
#include "stdlib.h"
#include <stdio.h>

int main() {
  int n = 0;
  Abonent *catalog = NULL;

  int choice;
  do {
    printf("Меню:\n1) Добавить абонента\n2) Удалить абонента\n3) Поиск "
           "абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
    scanf("%d", &choice);
    switch (choice) {
    case 1:
      if (catalog == NULL)
        create_catalog(&catalog, &n);
      else
        add_catalog(&catalog, &n);
      break;
    case 2:
      remove_catalog(&catalog, &n);
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
  free(catalog);
  return 0;
}
