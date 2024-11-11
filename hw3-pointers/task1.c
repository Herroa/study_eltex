#include <stdio.h>

int main() {
  unsigned value = -5;
  unsigned new_value = 9;
  unsigned char *ptr = (unsigned char *)&value;
  ptr[2] = new_value;
  for (int i = sizeof(int) * 8 - 1; i >= 0; i--) {
    printf("%d", value >> i & 1);
  }
  printf("\n");
  return 0;
}