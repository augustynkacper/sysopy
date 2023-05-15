#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <fcntl.h>         
#include <sys/stat.h>       
#include <semaphore.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "common.h"

sem_t *sem_queue, *sem_barbers, *sem_chairs, *sem_push;

sem_t* setup_semaphore(char*, int);
void close_semaphores();
sem_t* open_semaphore(char*);


int main(){
    
    int decriptor = shm_open(SHARED_QUEUE_NAME, O_CREAT | O_RDWR, 0600);
    ftruncate(decriptor, BUFF_SIZE);
    char *queue = (char*) mmap(0, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, decriptor, 0);

    queue[0] = '\0';

    sem_unlink(QUEUE_SEM);
    sem_unlink(BARBER_SEM);
    sem_unlink(CHAIRS_SEM);
    sem_unlink(PUSH_SEM);

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
    shmdt(queue);
    shmctl(decriptor, IPC_RMID, NULL);
}


sem_t* setup_semaphore(char* name, int size){
    sem_t* semaphore = sem_open(name, O_CREAT | O_EXCL,0644, size);
    if (semaphore==SEM_FAILED)printf("error\n");
    return semaphore;
}

sem_t* open_semaphore(char* name){
    sem_t* semaphore = sem_open(name, 0);
    if (semaphore==SEM_FAILED)printf("error\n");
    return semaphore;
}

void close_semaphore(sem_t* sem){
    sem_close(sem);
}

void close_semaphores(){
    sem_queue = open_semaphore(QUEUE_SEM);
    sem_barbers = open_semaphore(BARBER_SEM);
    sem_chairs = open_semaphore(CHAIRS_SEM);
    sem_push = open_semaphore(PUSH_SEM);
    close_semaphore(sem_queue);
    close_semaphore(sem_barbers);
    close_semaphore(sem_chairs);
    close_semaphore(sem_push);
}

