#define MAX_ABONENTS 100

typedef struct abonent {
  char name[10];
  char second_name[10];
  char tel[10];
} Abonent;

void add_catalog(Abonent catalog[], int *n);

void remove_catalog(Abonent catalog[], int n);

void search_catalog(Abonent catalog[], int n);

void print_catalog(Abonent catalog[], int n);
