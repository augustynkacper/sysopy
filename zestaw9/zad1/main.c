#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
 #include <stdlib.h>

#define REINDEERS_NUM 9
#define ELVES_NUM 10
#define ELVES_TO_WAKE 3
#define DELIVERS_TO_MAKE 2


pthread_t santa_thread;
pthread_t reindeer_threads[REINDEERS_NUM];
pthread_t elves_threads[ELVES_NUM];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t delivery_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_cond = PTHREAD_COND_INITIALIZER;

int elves = 0;
int reindeers = 0;
int deliveries = 0;
int elves_id[ELVES_TO_WAKE+1];

void *santa(void *arg){

    while( deliveries < DELIVERS_TO_MAKE ){

        pthread_mutex_lock(&mutex);


        while (reindeers < REINDEERS_NUM && elves < ELVES_TO_WAKE){
            printf("Mikolaj: zasypiam\n");
            pthread_cond_wait(&santa_cond, &mutex);
            printf("Mikolaj: budze sie\n");
        }

        if ( reindeers == REINDEERS_NUM ) {
            deliveries ++;
            printf("Mikolaj: dostarczam zabawki %d raz\n", deliveries);

            pthread_mutex_unlock(&mutex);
            usleep((rand() % 2001 + 2000) * 1000);
            pthread_mutex_lock(&mutex);

            pthread_cond_broadcast(&delivery_cond);
            reindeers = 0;

            if ( deliveries == DELIVERS_TO_MAKE){
                printf("Mikolaj: wykonalem %d dostaw prezentow\n", DELIVERS_TO_MAKE);
                pthread_exit( (void*) NULL);
            }
        }

        else if (elves == ELVES_TO_WAKE) {
            pthread_cond_broadcast(&elves_cond);
            printf("Mikolaj: rozwiazuje problemy elfow: ");
            for (int i=1; i<=ELVES_TO_WAKE; i++){
                printf(" %d", elves_id[i]);
            } printf("\n");

            pthread_mutex_unlock(&mutex);
            usleep((rand() % 1001 + 1000) * 1000);
            pthread_mutex_lock(&mutex);
            elves = 0;
            pthread_cond_broadcast(&elves_cond);

        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit((void*) NULL);
}

void *reindeer(void *arg){

    pthread_t id = pthread_self();

    while (1) {
        usleep((rand() % 5001 + 5000) * 1000);
        pthread_mutex_lock(&mutex);
        reindeers++;
        printf("Renifer: czeka %d renifeorow na mikolaja, ID: %ld\n", reindeers, id);

        if ( reindeers == REINDEERS_NUM ){
            printf("Renifer: budze mikolaja, ID: %ld\n", id);
            pthread_cond_signal(&santa_cond);
        }

        while ( reindeers > 0 ){
            pthread_cond_wait( &delivery_cond, &mutex );
        }

        pthread_mutex_unlock(&mutex);
    }
}

void *elf(void *arg){
    pthread_t id = pthread_self();

    while (1) {
   
        usleep((rand() % 3001 + 2000) * 1000);
        pthread_mutex_lock(&mutex);
        int elf_index = 0;

        if (elves<ELVES_TO_WAKE){
            elves++;
            elves_id[elves] = id;

            printf("Elf: na mikolaja czeka %d elfow, ID: %ld\n", elves, id);

            if (elves==ELVES_TO_WAKE){
                printf("Elf: budze mikolaja\n");
                pthread_cond_signal(&santa_cond);
            }
            pthread_cond_wait(&elves_cond, &mutex);
        } else {
            printf("Elf: samodzielnie rozwiazuje swoj problem, ID: %ld\n", id);
        }
        pthread_mutex_unlock(&mutex);
    }
}


int main(){
    pthread_create(&santa_thread, NULL, santa, (void*)NULL);

    for (int i=0; i<REINDEERS_NUM; i++){
        pthread_create(&reindeer_threads[i], NULL, reindeer, (void*)NULL);
    }

    for (int i=0; i<ELVES_NUM; i++ ){
        pthread_create(&elves_threads[i], NULL, elf, (void*)NULL);
    }

    pthread_join(santa_thread, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&santa_cond);
    pthread_cond_destroy(&elves_cond);
    pthread_cond_destroy(&delivery_cond);
}