#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define SHM_NAME "/my_shared_memory"
#define SEM_SERVER_NAME "/sem_server"
#define SEM_CLIENT_NAME "/sem_client"
#define SHM_SIZE 1024

typedef struct {
  char message[256];
} shared_data_t;

int main() {
  int shm_fd;
  shared_data_t *shared_data;
  sem_t *sem_server, *sem_client;

  printf("Client started...\n");

  sem_server = sem_open(SEM_SERVER_NAME, 0);
  sem_client = sem_open(SEM_CLIENT_NAME, 0);

  if (sem_server == SEM_FAILED || sem_client == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  shared_data =
      mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (shared_data == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  printf("Waiting for server message...\n");
  sem_wait(sem_server);

  printf("Client received: %s\n", shared_data->message);

  strcpy(shared_data->message, "Hello!");
  printf("Client sent: %s\n", shared_data->message);

  sem_post(sem_client);

  if (munmap(shared_data, SHM_SIZE) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  close(shm_fd);

  sem_close(sem_server);
  sem_close(sem_client);

  printf("Client finished\n");

  return 0;
}
