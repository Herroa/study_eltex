#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_KEY 12345
#define SEM_SERVER_KEY 12346
#define SEM_CLIENT_KEY 12347
#define SHM_SIZE 1024

typedef struct {
  char message[256];
} shared_data_t;

int main() {
  int shm_id, sem_server_id, sem_client_id;
  shared_data_t *shared_data;
  struct sembuf sem_buf;

  printf("Client started...\n");

  sem_server_id = semget(SEM_SERVER_KEY, 1, 0666);
  sem_client_id = semget(SEM_CLIENT_KEY, 1, 0666);

  if (sem_server_id == -1 || sem_client_id == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
  if (shm_id == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  shared_data = shmat(shm_id, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  printf("Waiting for server message...\n");

  sem_buf.sem_num = 0;
  sem_buf.sem_op = -1;
  sem_buf.sem_flg = 0;
  semop(sem_server_id, &sem_buf, 1);

  printf("Client received: %s\n", shared_data->message);

  strcpy(shared_data->message, "Hello!");
  printf("Client sent: %s\n", shared_data->message);

  sem_buf.sem_num = 0;
  sem_buf.sem_op = 1;
  sem_buf.sem_flg = 0;
  semop(sem_client_id, &sem_buf, 1);

  if (shmdt(shared_data) == -1) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }

  printf("Client finished\n");

  return 0;
}