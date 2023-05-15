#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>        
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>      
#include <fcntl.h> 

#include "common.h"

sem_t *sem_queue, *sem_barbers, *sem_chairs, *sem_push;

sem_t* open_semaphore(char* name){
    sem_t* semaphore = sem_open(name, 0);
    return semaphore;
}


void open_semaphores(){
    sem_queue = open_semaphore(QUEUE_SEM);
    sem_barbers = open_semaphore(BARBER_SEM);
    sem_chairs = open_semaphore(CHAIRS_SEM);
    sem_push = open_semaphore(PUSH_SEM);
}
void semaphore_operation(sem_t *sem, int val){
    if (val==1) sem_post(sem);
    else sem_wait(sem);
}


char pop(char* queue){
    char res = queue[0];
    for (int i=0; i<strlen(queue)-1; i++)
        queue[i] = queue[i+1];
    queue[strlen(queue)-1] = '\0';
    return res;
}

int main(){

    int decriptor = shm_open(SHARED_QUEUE_NAME, O_RDWR, 0644);
    ftruncate(decriptor, BUFF_SIZE);
    char *queue = (char*) mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, decriptor, 0);


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


