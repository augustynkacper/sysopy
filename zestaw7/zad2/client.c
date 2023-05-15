#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>      
#include <sys/stat.h>        
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>      
#include <fcntl.h> 

#include "common.h"

sem_t *sem_queue, *sem_barbers, *sem_chairs, *sem_push;

sem_t* open_semaphore(char* name){
    sem_t* semaphore = sem_open(name, 0);
    if(semaphore == SEM_FAILED) printf("error\n");
    return semaphore;
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

void semaphore_operation(sem_t *sem, int val){
    if (val==1) sem_post(sem);
    else if (sem_trywait(sem)==-1){
        printf("Client %d left the barbershop. (no places available)\n", getpid());exit(1);
    }
}



int main(){

    srand(time(NULL)+getpid());

    printf("Client %d entered the barbershop.\n", getpid()); fflush(stdout);
    
    int decriptor = shm_open(SHARED_QUEUE_NAME, O_RDWR, 0644);
    ftruncate(decriptor, BUFF_SIZE);
    char *queue = (char*) mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, decriptor, 0);
    
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