#ifndef COMMON_H
#define COMMON_H

#include <time.h>

#define PROJECT_ID 1
#define MAX_COMMAND_LENGTH 256
#define MAX_NO_CLIENTS 5
#define MAX_MESSAGES 10
#define QUEUE_NAME "/myqueue"
#define QUEUE_NAME_LENGTH 7

typedef enum msg_type {
    INIT = 1,
    TO_ALL = 2,
    TO_ONE = 3,
    LIST = 4,
    STOP = 5
} msg_type;

typedef struct msg_buffer {
    long type;
    int receiver_id;
    int client_id;
    key_t queue_key;
    char message[512];
    struct tm time_info;

} msg_buffer;


#endif 