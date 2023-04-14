#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include "common.h"

int clients[MAX_NO_CLIENTS];

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
            default:
                break;
        }

    fprintf(report_file, "\n\n");
    fclose(report_file);
}
void handle_INIT(){
    int i = 0;
    while (i<MAX_NO_CLIENTS && clients[i]!=-1) i++;
    if (i>=MAX_NO_CLIENTS) i = -1;

    clients[i] = msg_buff.queue_key;

    msg_buff.client_id = i;
    save_logs();
    printf("[SERVER] client initialized, queue_key: %d, client_int: %d\n", msg_buff.queue_key, i);
    int client_queue = msgget(msg_buff.queue_key, 0);
    msgsnd(client_queue, &msg_buff, sizeof(msg_buff), 0);
}


void handle_LIST(int id){
    printf("[SERVER] %d requested listing all clients\n", id);
    int i=1;
    msg_buffer mb;
    strcpy(mb.message, "connected client ids: 0");
    while (i<MAX_NO_CLIENTS && clients[i]!=-1){
        sprintf(mb.message+strlen(mb.message), ", %d", i);
        i ++;
    }
    mb.type = LIST;
    
    int client_queue = msgget(clients[id], 0);
    
    if (msgsnd(client_queue, &mb, sizeof(msg_buffer), 0)==-1)
        printf("error while sending!\n");

    save_logs();
}

void handle_TO_ALL(int id, char *mess){
    struct tm time_info = msg_buff.time_info;
    printf("[SERVER] %d sent message to everyone at %d:%d:%d\n", msg_buff.client_id, time_info.tm_hour, time_info.tm_min, time_info.tm_sec);

    msg_buffer mb;
    strcpy(mb.message, mess);
    mb.client_id = id;
    mb.type = TO_ALL;
    mb.time_info = msg_buff.time_info;

    int i = 0, client_queue;
    while (i<MAX_NO_CLIENTS && clients[i]!=-1){
        if (i==id) {i++; continue;}
        
        if((client_queue = msgget(clients[i], 0))==-1)
            printf("error while creating %d client queue\n", i);
        if(msgsnd(client_queue, &mb, sizeof(msg_buffer), 0)==-1)
            printf("error while sending!\n");
      
        i ++;
    }
    save_logs();
}

void handle_TO_ONE(int snd_id, int rcv_id, char* mess){
    save_logs();
    printf("[SERVER] %d sent a message to %d\n", snd_id,rcv_id);
    int receiver_queue = msgget(clients[rcv_id], 0);
    msgsnd(receiver_queue, &msg_buff, sizeof(msg_buffer), 0);
}


void handle_STOP(){
    save_logs();
    printf("[SERVER] %d disconnected\n", msg_buff.client_id);
    clients[msg_buff.client_id] = -1;
}

int main(){


    for (int i=0; i<MAX_NO_CLIENTS;i++)
        clients[i] = -1;
    

    char* home_path = getenv("HOME");
    
    key_t key = ftok(home_path, PROJECT_ID);

    int server_queue;

    if((server_queue = msgget(key, IPC_CREAT | 0600)) == -1){
        printf("error during creating server queue!\n"); 
        exit(1);
    }
    
    printf("[SERVER] Server is ready. Queue key: %d\n", key);



    while(1){
        msgrcv(server_queue, &msg_buff, sizeof(msg_buffer), -100, 0);


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