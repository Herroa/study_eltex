#include "../include/calc.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  int a = 0, b = 0;

  int choice;
  do {
    printf("menu:\n1) add\n2) sub\n3) mul "
           "\n4) div\n5) exit\n");
    scanf("%d", &choice);
    switch (choice) {
    case 1:
      printf("input operands: \n");
      scanf("%d %d", &a, &b);
      printf("%d\n", add(a, b));
      break;
    case 2:
      printf("input operands: \n");
      scanf("%d %d", &a, &b);
      printf("%d\n", sub(a, b));
      break;
    case 3:
      printf("input operands: \n");
      scanf("%d %d", &a, &b);
      printf("%d\n", mul(a, b));
      break;
    case 4:
      printf("input operands: \n");
      scanf("%d %d", &a, &b);
      if (b == 0)
        break;
      printf("%d\n", my_div(a, b));
      break;
    case 5:
      printf("Завершение программы.\n");
      break;
    }
  } while (choice != 5);
  return 0;
}
