#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_ROOMS 5
#define INITIAL_STOCK 500
#define NUM_CUSTOMERS 3
#define MAX_DEMAND 12000
#define MIN_DEMAND 10000

int rooms[NUM_ROOMS];
pthread_mutex_t room_mutexes[NUM_ROOMS];

void* customer(void* arg) {
  int id = *((int*)arg);
  int demand = rand() % (MAX_DEMAND - MIN_DEMAND + 1) + MIN_DEMAND;

  printf("Покупатель %d: Проснулся, начальная потребность = %d\n", id, demand);

  while (demand > 0) {
    int room = rand() % NUM_ROOMS;
    pthread_mutex_lock(&room_mutexes[room]);

    if (rooms[room] > 0) {
      int bought = (rooms[room] > demand) ? demand : rooms[room];
      rooms[room] -= bought;
      demand -= bought;
      printf(
          "Покупатель %d: В комнате %d купил %d товаров, осталась потребность "
          "= %d\n",
          id, room, bought, demand);
    } else {
      printf("Покупатель %d: В комнате %d нет товаров, ухожу.\n", id, room);
    }

    pthread_mutex_unlock(&room_mutexes[room]);

    if (demand > 0) {
      printf(
          "Покупатель %d: Осталась потребность = %d, засыпаю на 2 секунды...\n",
          id, demand);
      sleep(2);
    }
  }

  printf("Покупатель %d: Все товары куплены, завершаю работу.\n", id);
  return NULL;
}

void* loader(void* arg) {
  while (1) {
    int room = rand() % NUM_ROOMS;
    pthread_mutex_lock(&room_mutexes[room]);

    rooms[room] += 500;
    printf("Погрузчик: Добавил 500 товаров в комнату %d, новый запас = %d\n",
           room, rooms[room]);

    pthread_mutex_unlock(&room_mutexes[room]);
    sleep(1);
  }

  return NULL;
}

int main() {
  srand(time(NULL));

  for (int i = 0; i < NUM_ROOMS; i++) {
    rooms[i] = INITIAL_STOCK;
    pthread_mutex_init(&room_mutexes[i], NULL);
  }

  pthread_t customers[NUM_CUSTOMERS], loader_thread;
  int customer_ids[NUM_CUSTOMERS];

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    customer_ids[i] = i + 1;
    pthread_create(&customers[i], NULL, customer, &customer_ids[i]);
  }

  pthread_create(&loader_thread, NULL, loader, NULL);

  for (int i = 0; i < NUM_CUSTOMERS; i++) {
    pthread_join(customers[i], NULL);
  }

  pthread_cancel(loader_thread);

  printf(
      "Главный поток: Все покупатели завершили работу, программа "
      "завершается.\n");

  for (int i = 0; i < NUM_ROOMS; i++) {
    pthread_mutex_destroy(&room_mutexes[i]);
  }

  return 0;
}