#include "stdio.h"

int main() {
  // 1,2
  int value;
  printf("Введите целое число: ");
  scanf("%d", &value);
  printf("Представление числа %d в двоичном виде:\n", value);
  for (int i = sizeof(int) * 8; i > 0; i--) {
    printf("%d", value >> i & 1);
    if ((i - 1) % 8 == 0)
      printf(" ");
  }
  printf("\n");
  // 3
  int value2, counter = 0;
  printf("Введите целое число: ");
  scanf("%d", &value2);
  for (int i = sizeof(int) * 8; i > 0; i--) {
    if ((value2 >> i & 1) == 1)
      counter++;
  }
  printf("Количество единиц в числе %d = %d\n", value2, counter);
  // 4
  int value3, insert_value;
  printf("Введите целое число: ");
  scanf("%d", &value3);
  printf("Введите число для вставки в третий бит: ");
  scanf("%d", &insert_value);
  printf("Начальное число:\n");
  for (int i = sizeof(int) * 8; i > 0; i--) {
    printf("%d", value3 >> i & 1);
    if ((i - 1) % 8 == 0)
      printf(" ");
  }
  printf("\n");
  value3 = value3 & 0xFF00FFFF;
  value3 = value3 | (insert_value << 17);
  printf("Число после вставки:\n");
  for (int i = sizeof(int) * 8; i > 0; i--) {
    printf("%d", value3 >> i & 1);
    if ((i - 1) % 8 == 0)
      printf(" ");
  }
  printf("\n");

  return 0;
}
