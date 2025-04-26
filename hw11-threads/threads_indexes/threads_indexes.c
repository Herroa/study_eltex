#include <pthread.h>
#include <stdio.h>

#define N 5

void *thread_calc(void *args) {
  int *i = (int *)args;
  printf("thread num %d\n", *i);
  return NULL;
}

int main() {
  pthread_t thread[N];
  int data[N];
  int i = 0;
  int *s;

  for (; i < N; i++) {
    data[i] = i;
    pthread_create(&thread[i], NULL, thread_calc, (void *)&data[i]);
  }

  for (int j = 0; j < N; j++) {
    pthread_join(thread[j], (void *)&s);
  }
  return 0;
}