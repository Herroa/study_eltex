#include <stdio.h>
#define n 10

int main() {
  int array[n] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int *ptr = &array[0];
  for (int i = 0; i < n; i++)
    printf("%d\n", *(ptr + i));
  printf("\n");
  return 0;
}