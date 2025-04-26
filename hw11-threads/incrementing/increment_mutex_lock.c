#include <pthread.h>
#include <stdio.h>
#include <time.h>

#define NUM_THREADS 4
#define N 180000000

long a = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment(void* arg) {
  int local_N = N / NUM_THREADS;
  for (int i = 0; i < local_N; i++) {
    pthread_mutex_lock(&mutex);
    a++;
    pthread_mutex_unlock(&mutex);
  }

  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, increment, NULL);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}