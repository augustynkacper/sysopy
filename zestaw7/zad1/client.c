#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <string.h>
#include <time.h>

#include "common.h"

int sem_queue, sem_barbers, sem_chairs, sem_push;

int open_semaphore(char name){
    key_t key = ftok(KEY_PATH, name);
    return semget(key, 1, 0);
}

void open_semaphores(){
    sem_queue = open_semaphore(QUEUE_SEM);
    sem_barbers = open_semaphore(BARBER_SEM);
    sem_chairs = open_semaphore(CHAIRS_SEM);
    sem_push = open_semaphore(PUSH_SEM);
}

void push(char* queue, char element){
    int length = strlen(queue);
    queue[length] = element;
    queue[length+1] = '\0';
}

void semaphore_operation(int sem_id, int val){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = val;
    sb.sem_flg = IPC_NOWAIT;
    if (semop(sem_id, &sb, 1)==-1){
        printf("Client %d left the barbershop. (no places available)\n", getpid());
        exit(1);
    }
    
}


int main(){

    srand(time(NULL)+getpid());

    printf("Client %d entered the barbershop.\n", getpid()); fflush(stdout);

    key_t key = ftok(KEY_PATH, 0);
    int shared_mem_id = shmget(key, BUFF_SIZE, IPC_CREAT | 0600);
    char* queue = shmat(shared_mem_id, NULL, 0);
    //printf("client queue after push %s\n", queue); fflush(stdout);
    open_semaphores();

    // adding a client to the queue (in barber shop)
    semaphore_operation(sem_queue, -1);
    int rand_num = rand() % 5;
    char haircut = '1'+rand_num;
 // pushing to the queue (in our program)
    semaphore_operation(sem_push, -1);
    push(queue, haircut);
    semaphore_operation(sem_push, 1);
    
    shmdt(queue);
}