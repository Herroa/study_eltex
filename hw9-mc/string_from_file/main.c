#include <stdio.h>

int main(void) {
  char *message = "String from file\n";
  char *filename = "output.txt";
  FILE *fp = fopen(filename, "w");
  if (fp) {
    fputs(message, fp);
    fclose(fp);
  }

  char buffer[256];
  fp = fopen(filename, "r");
  if (fp) {
    while ((fgets(buffer, 256, fp)) != NULL)
      printf("%s", buffer);
    fclose(fp);
  }
}