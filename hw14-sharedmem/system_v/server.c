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

union semun {
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

int main() {
  int shm_id, sem_server_id, sem_client_id;
  shared_data_t *shared_data;
  union semun sem_union;
  struct sembuf sem_buf;

  printf("Server started...\n");

  sem_server_id = semget(SEM_SERVER_KEY, 1, IPC_CREAT | 0666);
  sem_client_id = semget(SEM_CLIENT_KEY, 1, IPC_CREAT | 0666);

  if (sem_server_id == -1 || sem_client_id == -1) {
    perror("semget");
    exit(EXIT_FAILURE);
  }

  sem_union.val = 0;
  semctl(sem_server_id, 0, SETVAL, sem_union);
  semctl(sem_client_id, 0, SETVAL, sem_union);

  shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
  if (shm_id == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  shared_data = shmat(shm_id, NULL, 0);
  if (shared_data == (void *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  strcpy(shared_data->message, "Hi!");
  printf("Server sent: %s\n", shared_data->message);

  sem_buf.sem_num = 0;
  sem_buf.sem_op = 1;
  sem_buf.sem_flg = 0;
  semop(sem_server_id, &sem_buf, 1);

  printf("Waiting for client response...\n");

  sem_buf.sem_num = 0;
  sem_buf.sem_op = -1;
  sem_buf.sem_flg = 0;
  semop(sem_client_id, &sem_buf, 1);

  printf("Server received: %s\n", shared_data->message);

  if (shmdt(shared_data) == -1) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }

  semctl(sem_server_id, 0, IPC_RMID, sem_union);
  semctl(sem_client_id, 0, IPC_RMID, sem_union);

  printf("Server finished\n");

  return 0;
}