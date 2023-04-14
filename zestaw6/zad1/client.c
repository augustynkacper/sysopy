#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "common.h"


key_t server_key, client_key;
int server_queue, client_queue, client_id;

msg_buffer msg_buff;

void handle_INIT(){
    time_t current_time = time(NULL);

    msg_buff.time_info = *localtime(&current_time);
    msg_buff.type = INIT;
    msg_buff.queue_key = client_key;
    msgsnd(server_queue, &msg_buff, sizeof(msg_buffer), 0);
    msgrcv(client_queue, &msg_buff, sizeof(msg_buffer), 0, 0);
    client_id = msg_buff.client_id; 
    printf("[CLIENT] client initialized, queue_key: %d, client_id:%d\n", client_key, client_id);
}

void handle_LIST(){
    time_t current_time = time(NULL);

    msg_buff.time_info = *localtime(&current_time);
    msg_buff.type = LIST;
    msg_buff.client_id = client_id;

    if(msgsnd(server_queue, &msg_buff, sizeof(msg_buffer), 0)==-1)
        printf("error while sending...\n");

    if(msgrcv(client_queue, &msg_buff, sizeof(msg_buffer),0,0)==-1)
        printf("error while receiving...\n");

    printf("%s\n",msg_buff.message);
}

void handle_TO_ALL(char* mess){
    time_t current_time = time(NULL);

    msg_buff.time_info = *localtime(&current_time);
    msg_buff.type = TO_ALL;
    msg_buff.client_id = client_id;
    strcpy(msg_buff.message, mess);
    if (msgsnd(server_queue, &msg_buff, sizeof(msg_buffer), 0)==-1)
        printf("error while sending...\n");
}
void handle_TO_ONE(int rcv_id, char* message){
    time_t current_time = time(NULL);

    msg_buff.time_info = *localtime(&current_time);
    msg_buff.type = TO_ONE;
    strcpy(msg_buff.message, message);
    msg_buff.client_id = client_id;
    msg_buff.receiver_id = rcv_id;

    msgsnd(server_queue, &msg_buff, sizeof(msg_buffer), 0);
}

void handle_STOP(){
    time_t current_time = time(NULL);

    msg_buff.time_info = *localtime(&current_time);
    msg_buff.type = STOP;
    msg_buff.client_id = client_id;

    msgsnd(server_queue, &msg_buff, sizeof(msg_buffer), 0);
    msgctl(client_queue, IPC_RMID, NULL);
    //msgctl(server_queue, IPC_RMID, NULL);
    exit(0);
}

void handle_server_message(){
    while (msgrcv(client_queue, &msg_buff, sizeof(msg_buffer), 0, IPC_NOWAIT) >=0 ){
        if (msg_buff.type == TO_ALL || msg_buff.type == TO_ONE){
            struct tm time_info = msg_buff.time_info;
            printf("Received message '%s' from %d, sent at %d:%d:%d\n", msg_buff.message,msg_buff.client_id, time_info.tm_hour, time_info.tm_min, time_info.tm_sec);
        }
    }
}

int main(){
    signal(SIGINT, handle_STOP);
    char* home_path = getenv("HOME");

    server_key = ftok(home_path, PROJECT_ID);
    server_queue = msgget(server_key, 0);

    client_key = ftok(home_path, getpid());
    client_queue = msgget(client_key, IPC_CREAT | 0600);

    printf("%d\n", client_key);
    // initializing a client
    handle_INIT();

    // reading commands, and sending them to a server
    int read_size;
    char* command = calloc(COMMAND_LENGTH, sizeof(char));
    char* token = calloc(255, sizeof(char));
    while(1){
        printf("> ");  fgets(command, COMMAND_LENGTH, stdin);
        
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