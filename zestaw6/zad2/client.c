#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <mqueue.h>
#include <time.h>
#include "common.h"


key_t server_key, client_key;
mqd_t server_queue, client_queue;
int client_id;

msg_buffer msg_buff;
char queue_name[QUEUE_NAME_LENGTH];
void update_time( msg_buffer *mb ){
    time_t ct = time(NULL);
    mb->time_info=*localtime(&ct);
}

void handle_INIT(){
    update_time(&msg_buff);

    msg_buff.type = INIT;
    strcpy(msg_buff.message, queue_name);

    mq_send(server_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, 0);
    mq_receive(client_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, NULL);

    client_id = msg_buff.client_id; 
    printf("[CLIENT] client initialized, id: %d\n", client_id);
}

void handle_LIST(){
    update_time(&msg_buff);
   
    msg_buff.type = LIST;
    msg_buff.client_id = client_id;

    if (mq_send(server_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, 0)<0)
        printf("list - sending error!\n");

    mq_receive(client_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, NULL);

    printf("%s\n",msg_buff.message);
}

void handle_TO_ALL(char* mess){
    update_time(&msg_buff);

    msg_buff.type = TO_ALL;
    msg_buff.client_id = client_id;
    strcpy(msg_buff.message, mess);

    if (mq_send(server_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, 0) == -1)
        printf("2all - sending error!\n");
}
void handle_TO_ONE(int rcv_id, char* message){
    update_time(&msg_buff);

    msg_buff.type = TO_ONE;

    strcpy(msg_buff.message, message);

    msg_buff.client_id = client_id;
    msg_buff.receiver_id = rcv_id;

    if (mq_send(server_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, 0)<0)
        printf("2one - sending error!\n");
}

void handle_STOP(){
    update_time(&msg_buff);

    msg_buff.type = STOP;
    msg_buff.client_id = client_id;

    mq_send(server_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, 0);
    mq_close(server_queue);
    
    exit(0);
}


void handle_server_message(){
    struct timespec my_time;
    clock_gettime(CLOCK_REALTIME, &my_time);
    my_time.tv_sec += 0.1;
    while (mq_timedreceive(client_queue, (char*) &msg_buff, MAX_COMMAND_LENGTH, NULL, &my_time) != -1 ){
        if (msg_buff.type == TO_ALL || msg_buff.type == TO_ONE){
            struct tm time_info = msg_buff.time_info;
            printf("Received message '%s' from %d, sent at %d:%d:%d\n", msg_buff.message,msg_buff.client_id, time_info.tm_hour, time_info.tm_min, time_info.tm_sec);
        }
    }
}

int main(){
    srand(time(NULL));
    signal(SIGINT, handle_STOP);
    
    queue_name[0]='/';
    for (int i=1;i<QUEUE_NAME_LENGTH-1;i++) 
        queue_name[i] = rand() % ('z'-'a'+1) + 'a';


    struct mq_attr attr; 
    attr.mq_msgsize = MAX_COMMAND_LENGTH;
    attr.mq_maxmsg = MAX_MESSAGES;

    client_queue = mq_open(queue_name, O_RDONLY | O_CREAT, 0666, &attr);
    server_queue = mq_open(QUEUE_NAME, O_RDWR);



    // initializing a client
    handle_INIT();

    // reading commands, and sending them to a server
    char* command = calloc(MAX_COMMAND_LENGTH, sizeof(char));

    while(1){
        printf("> ");  fgets(command, MAX_COMMAND_LENGTH, stdin);
        
        char* token = strtok(command, " ");
        if (token[strlen(token)-1]=='\n')
            token[strlen(token)-1]='\0';
     
        handle_server_message();

        if ( strcmp(token, "LIST") == 0){
            handle_LIST();
        } else if ( strcmp(token, "2ALL") == 0){
            char* arg = strtok(NULL, "");
            arg[strlen(arg)-1] = '\0';
            handle_TO_ALL(arg);
        } else if ( strcmp(token, "2ONE") == 0){
            char* id_char = strtok(NULL, " ");
            int rcv_id = atoi(id_char);
            char *mess = strtok(NULL, "");
            mess[strlen(mess)-1] = '\0';
            handle_TO_ONE(rcv_id, mess);
        } else if ( strcmp(token, "STOP") == 0){
            handle_STOP();
        } else{
            printf("Wrong command!\n");
        }
    }

    return 0;
}