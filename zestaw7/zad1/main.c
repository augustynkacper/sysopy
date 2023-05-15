#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/wait.h>

#include "common.h"

int sem_queue, sem_barbers, sem_chairs, sem_push;

int setup_semaphore(char, int);
void close_semaphores();
int open_semaphore(char);


int main(){

    key_t key = ftok(KEY_PATH, 0);
    int shared_mem_id = shmget(key, BUFF_SIZE, IPC_CREAT | 0600);
    char* queue = shmat(shared_mem_id, NULL, 0);
    queue[0] = '\0';

    close_semaphores();

    sem_queue = setup_semaphore(QUEUE_SEM, QUEUE_SIZE);
    sem_barbers = setup_semaphore(BARBER_SEM, BARBERS_NUMBER);
    sem_chairs = setup_semaphore(CHAIRS_SEM, CHAIRS_NUMBER);
    sem_push = setup_semaphore(PUSH_SEM, 1);
 
    
    for (int i=0; i<CLIENTS_NUMBER; i++){
        if (fork() == 0) {
            execl("./client", "./client", NULL);
        }
    }
        
    for (int i=0; i<BARBERS_NUMBER; i++){
        if (fork() == 0) {
            execl("./barber", "./barber", NULL);
        }
    }
    
    printf("Barbers created...\n"); fflush(stdout);

    
    
    printf("Clients created...\n"); fflush(stdout);

    while(wait(NULL)>0);

    close_semaphores();
    shmctl(shared_mem_id, IPC_RMID, NULL);
}


int setup_semaphore(char name, int size){
    key_t key = ftok(KEY_PATH, name);

    int sem_id = semget(key, 1, IPC_CREAT | 0600);

    semctl(sem_id, 0, SETVAL, size);
    return sem_id;
}

int open_semaphore(char name){
    key_t key = ftok(KEY_PATH, name);
    return semget(key, 1, 0);
}

void close_semaphore(char name){
    int sem_id = open_semaphore(name);
    semctl(sem_id, 0, IPC_RMID);
}

void close_semaphores(){
    sem_queue = open_semaphore(QUEUE_SEM);
    sem_barbers = open_semaphore(BARBER_SEM);
    sem_chairs = open_semaphore(CHAIRS_SEM);
    sem_push = open_semaphore(PUSH_SEM);
    close_semaphore(QUEUE_SEM);
    close_semaphore(CHAIRS_SEM);
    close_semaphore(BARBER_SEM);
    close_semaphore(PUSH_SEM);
}

