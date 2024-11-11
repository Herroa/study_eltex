#include <stdio.h>
#define n 5
#define n2 5
#define n3 5
#define n4 5

int main() {
  // 1
  int array[n][n], x = 1;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      array[i][j] = x;
      printf("%d ", array[i][j]);
      x++;
    }
    printf("\n");
  }
  // 2
  int array2[n2] = {1, 2, 3, 4, 5};
  int i = 0, j = n2 - 1, temp;
  while (i < j) {
    temp = array2[j];
    array2[j] = array2[i];
    array2[i] = temp;
    i++;
    j--;
    printf("i=%d, val=%d, j=%d, val=%d.\n", i, array2[i], j, array2[j]);
  }

  for (int i = 0; i < n2; i++) {
    printf("%d ", array2[i]);
  }
  printf("\n");
  // 3
  int array3[n3][n3];
  for (int i = 0; i < n3; i++) {
    for (int j = 0; j < n3; j++) {
      if (i + j >= n3 - 1)
        array3[i][j] = 1;
      else
        array3[i][j] = 0;
      printf("%d ", array3[i][j]);
    }
    printf("\n");
  }
  // 4
  int array4[n4][n4];
  int x4 = 1;
  int top = 0, bottom = n4 - 1, left = 0, right = n4 - 1;

  while (x4 <= n4 * n4) {
    for (int i = left; i <= right; i++) {
      array4[top][i] = x4++;
    }
    top++;

    for (int i = top; i <= bottom; i++) {
      array4[i][right] = x4++;
    }
    right--;

    for (int i = right; i >= left; i--) {
      array4[bottom][i] = x4++;
    }
    bottom--;

    for (int i = bottom; i >= top; i--) {
      array4[i][left] = x4++;
    }
    left++;
  }

  for (int i = 0; i < n4; i++) {
    for (int j = 0; j < n4; j++) {
      printf("%2d ", array4[i][j]);
    }
    printf("\n");
  }
  return 0;
}