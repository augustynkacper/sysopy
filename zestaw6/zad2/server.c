#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include "common.h"
#include <mqueue.h>

char *clients[MAX_NO_CLIENTS];
msg_buffer msg_buff;


void save_logs(){

    FILE *report_file = fopen("raport.txt", "a");

     fprintf(report_file, "sent at: %d:%d:%d\n",
            msg_buff.time_info.tm_hour,
            msg_buff.time_info.tm_min,
            msg_buff.time_info.tm_sec);


    switch(msg_buff.type){
            case INIT:
                if (msg_buff.client_id==-1){
                    fprintf(report_file, "[INIT] max number of clients reached\n");
                } else {
                    fprintf(report_file, "[INIT] client connected, id: %d\n", msg_buff.client_id);
                } 
                break;
            case LIST:
                fprintf(report_file, "[LIST] client %d requested list\n", msg_buff.client_id);
                break;
            case TO_ALL:
                fprintf(report_file, "[2ALL] client %d sent '%s' to everyone\n", msg_buff.client_id, msg_buff.message);    
                break;
            case TO_ONE:
                fprintf(report_file, "[2ONE] client %d sent '%s' to %d\n",  msg_buff.client_id, msg_buff.message, msg_buff.receiver_id);
                break;
            case STOP:
                fprintf(report_file, "[STOP] client %d disconnected\n", msg_buff.client_id);
            default:
                break;
        }

    fprintf(report_file, "\n\n");
    fclose(report_file);
}


void handle_INIT(){

    int i = 0;
    while (i<MAX_NO_CLIENTS && clients[i]!=NULL) i++;

    if (i>=MAX_NO_CLIENTS) i = -1;

    msg_buff.client_id = i;

    clients[i] = calloc(QUEUE_NAME_LENGTH, sizeof(char));
    strcpy(clients[i], msg_buff.message);

    mqd_t c_q = mq_open(clients[i], O_RDWR);
    mq_send(c_q, (char *) &msg_buff, MAX_COMMAND_LENGTH, 0);
    mq_close(c_q);
   
    save_logs();
    printf("[SERVER] client initialized, queue_name: %s, client_id: %d\n", clients[i], i);
}


void handle_LIST(int id){
    printf("[SERVER] %d requested listing all clients\n", id);

    msg_buffer mb;
    strcpy(mb.message, "connected clients' queues:");

    for(int i=0; i<MAX_NO_CLIENTS; i++){
        if (clients[i]!=NULL)
            sprintf(mb.message+strlen(mb.message), " %s", clients[i]);
    }

    mqd_t c_q = mq_open(clients[id], O_RDWR);
    mq_send(c_q, (char *) &mb, MAX_COMMAND_LENGTH, 0);
    mq_close(c_q);

    save_logs();
}

void handle_TO_ALL(int id, char *mess){
    printf("[SERVER] %d sent '%s' to everyone\n", msg_buff.client_id, msg_buff.message);

    msg_buffer mb;
    strcpy(mb.message, mess);
    mb.client_id = id;
    mb.type = TO_ALL;

    for (int i=0; i<MAX_NO_CLIENTS; i++){
        if (i==id || clients[i]==NULL) continue;
        mqd_t c_q = mq_open(clients[i], O_RDWR);
        mq_send(c_q, (char *) &mb, MAX_COMMAND_LENGTH, 0);
        mq_close(c_q);
    }

    save_logs();
}

void handle_TO_ONE(int snd_id, int rcv_id, char* mess){
    save_logs();
    printf("[SERVER] %d sent a message to %d\n", snd_id,rcv_id);

    mqd_t c_q = mq_open(clients[rcv_id], O_RDWR);
    mq_send(c_q, (char *) &msg_buff, MAX_COMMAND_LENGTH, 0);
    mq_close(c_q);
}


void handle_STOP(){
    save_logs();
    printf("[SERVER] %d disconnected\n", msg_buff.client_id);
    clients[msg_buff.client_id] = NULL;
}

int main(){

    for (int i=0; i<MAX_NO_CLIENTS;i++)
        clients[i] = NULL;
    

    struct mq_attr attr; 
    attr.mq_msgsize = MAX_COMMAND_LENGTH;
    attr.mq_maxmsg = MAX_MESSAGES;

    int server_queue = mq_open(QUEUE_NAME, O_RDONLY | O_CREAT, 0666, &attr);
    
    
    printf("[SERVER] Server is ready. Queue name: %s\n", QUEUE_NAME);

    while(1){
        if(mq_receive(server_queue, (char *) &msg_buff, MAX_COMMAND_LENGTH, NULL)==-1)
            printf("[SERVER] error while receiving!\n");
        //printf("%d\n", msg_buff.idk);
        

        switch(msg_buff.type){
            case INIT:
                handle_INIT();
                 
                break;
            case LIST:
                handle_LIST(msg_buff.client_id);
                break;
            case TO_ALL:
                handle_TO_ALL(msg_buff.client_id, msg_buff.message);
                break;
            case TO_ONE:
                handle_TO_ONE(msg_buff.client_id, msg_buff.receiver_id, msg_buff.message);
                break;
            case STOP:
                handle_STOP();
                break;
            default:
                printf("WTF\n");
                break;
        }
    }
    

    return 0;
}