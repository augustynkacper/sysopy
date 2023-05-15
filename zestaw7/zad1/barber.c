#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>


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

void semaphore_operation(int sem_id, int val){
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = val;
    sb.sem_flg = 0;
    semop(sem_id, &sb, 1);
}

char pop(char* queue){
    char res = queue[0];
    for (int i=0; i<strlen(queue)-1; i++)
        queue[i] = queue[i+1];
    queue[strlen(queue)-1] = '\0';
    return res;
}

int main(){

    key_t key = ftok(KEY_PATH, 0);
    int shared_mem_id = shmget(key, BUFF_SIZE, IPC_CREAT | 0600);
    char* queue = shmat(shared_mem_id, NULL, 0);

    open_semaphores();

    //printf("Barber %s %d.\n", queue, getpid()); fflush(stdout);
    while (1){
        if (strlen(queue)==0) continue;
        semaphore_operation(sem_chairs, -1);
        semaphore_operation(sem_barbers, -1);
        semaphore_operation(sem_queue, 1);

        semaphore_operation(sem_push, -1);
        char haircut = pop(queue);
        semaphore_operation(sem_push, 1);
        if (haircut=='\0') continue;
        // occupy a chair
        sleep(haircut-'0');
        printf("[BARBER %d] client served...\n", getpid()); fflush(stdout);
        semaphore_operation(sem_chairs, 1);
        
        semaphore_operation(sem_barbers, 1);
        
    
        if (strlen(queue)==0) break;
        
    }


    shmdt(queue);
    
}


