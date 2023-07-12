#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE_LENGTH 64

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epoll_fd;

typedef struct {
    int fd;
    char name[10];
    int in_use;
    int online;
    int to_init;
    int ready;
    struct sockaddr *addr;
} client;

typedef struct event_data {
    enum event_type { new_connection, client_message } type;
    int socket;
    client* client;
} event_data;

client clients[MAX_CLIENTS];

int init_local_socket(char *);
int init_web_socket(int);
void *ping(void *);
void delete_client(client* client);
client* create_client(int fd, char* name, struct sockaddr *addr);

void send_message(char* msg, int fd, struct sockaddr *addr){
    sendto(fd, msg, MAX_MESSAGE_LENGTH, 0, (struct sockaddr*) addr, sizeof *addr);
}

void handler(int ){
    for (int i=0; i<MAX_CLIENTS; i++){
        if (clients[i].in_use == 1 ){
            //send_message("STOP", clients[i].fd);
            send_message("STOP", clients[i].fd, clients[i].addr);
            //send(clients[i].fd, "STOP", MAX_MESSAGE_LENGTH, 0);
        }
    }
    exit(1);
}

void on_client_message(client* client, char* buff, struct sockaddr *addr, int fd){

    char* cmd = strtok(buff, " ");

    if (strcmp(cmd, "CONNECTED")==0){
        char*name = strtok(NULL, " ");
        create_client(fd, name, addr);
    } else if (strcmp(cmd, "PING")==0){
        pthread_mutex_lock(&mutex);
        client->online = 1;
        pthread_mutex_unlock(&mutex);
    } else if ( strcmp(cmd, "STOP")==0){
        delete_client(client);
    } else if (strcmp(cmd, "2ALL")==0){
        char* arg = strtok(NULL, " ");
        char msg[256] = "";
        strcat(msg, client->name);
        strcat(msg, ": ");
        strcat(msg, arg);

        for (int i=0; i<MAX_CLIENTS; i++){
            if (clients[i].in_use ==1  && strcmp(clients[i].name, client->name) != 0 ){
                send_message(msg, clients[i].fd, client->addr);
                //send(clients[i].fd, msg, MAX_MESSAGE_LENGTH, 0);
            }
        }
    } else if (strcmp(cmd, "2ONE")==0){
        char* client_name = strtok(NULL, " ");
        char* arg = strtok(NULL, " ");
        char msg[256] = "";
        strcat(msg, client->name);
        strcat(msg, ": ");
        strcat(msg, arg);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].in_use == 1) {
                if (strcmp(clients[i].name, client_name) == 0) {
                    send_message(msg, clients[i].fd, client->addr);
                    //send(clients[i].fd, msg, MAX_MESSAGE_LENGTH, 0);
                }
            }
        }
    } else if (strcmp(cmd, "LIST")==0) {
        printf("LISTING\n");
        char msg[256] = "";
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].in_use == 1){
                strcat(msg, clients[i].name);
                strcat(msg, " ");
            }
        }
        send_message(msg, client->fd, client->addr);
        //send(client->fd, msg, MAX_MESSAGE_LENGTH, 0);
    }

}



int main(int argc, char *argv[]) {
    signal(SIGINT, handler);

    char *socket_path;
    int port;

    int local_socket, web_socket;

    port = atoi(argv[1]);
    socket_path = argv[2];

    printf("port: %d, socket_path: %s\n", port, socket_path);

    epoll_fd = epoll_create1(0);
    local_socket = init_local_socket(socket_path);
    web_socket = init_web_socket(port);

    printf("[WEB] %d\n", web_socket);
    printf("[LOCAL] %d\n", local_socket);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);

    struct epoll_event events[MAX_CLIENTS];

    while (1){
        printf("waiting\n\n");
        int events_occured = epoll_wait(epoll_fd, events, MAX_CLIENTS, -1);

        printf("connected users: ");
        for(int i=0; i<MAX_CLIENTS; i++){
            if (clients[i].in_use==1)
                printf("%s ", clients[i].name);
        }   printf("\n");



        for (int i=0; i<events_occured; i++){
            int fd = events[i].data.fd;

            struct sockaddr addr;
            socklen_t addr_size = sizeof addr;
            char buff[256];

            recvfrom(fd, buff, sizeof buff, 0, &addr, &addr_size);
            printf("buff: %s\n", buff);

            on_client_message(&clients[i], buff, &addr, fd);
        }
    }
}

int init_web_socket(int port){
    struct sockaddr_in web_address = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };
    int web_socket = socket(AF_INET, SOCK_STREAM, 0);

    if ( bind(web_socket, (struct sockaddr*) &web_address, sizeof(web_address)) == -1){
        printf("[ERROR] web socket binding\n"); exit(1);
    }
    //listen(web_socket, MAX_CLIENTS);
    struct epoll_event event = { .events = EPOLLIN | EPOLLPRI, .data = { .fd = web_socket }  };
    event_data* event_data = event.data.ptr = malloc(sizeof *event_data);
    event_data->type = new_connection;
    event_data->socket = web_socket;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, web_socket, &event);
    return web_socket;
}

int init_local_socket(char *socket_path){
    puts(socket_path);
    struct sockaddr_un local_address = { .sun_family = AF_UNIX };
    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    strncpy(local_address.sun_path, socket_path, sizeof(local_address.sun_path));
    if (bind(local_socket, (struct sockaddr *) &local_address, sizeof(struct sockaddr_un)) == -1){
        printf("[ERROR] local socket binding\n"); exit(1);
    }
    //listen(local_socket, MAX_CLIENTS);
    struct epoll_event event = { .events = EPOLLIN | EPOLLPRI , .data = { .fd = local_socket } };

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, local_socket, &event);
    return local_socket;
}

void *ping(void *_){
    while (1) {
        sleep(1);
        pthread_mutex_lock( &mutex );

        for (int i=0; i< MAX_CLIENTS; i++){
            if (clients[i].in_use == 1){
                if (clients[i].online != 1)
                    delete_client(&clients[i]);
                else {
                    clients[i].online = 0;
                    send_message("PING", clients[i].fd, clients[i].addr);
                    //send(clients[i].fd, "PING", MAX_MESSAGE_LENGTH, 0);
                }
            }
        }

        pthread_mutex_unlock( &mutex );
    }
}

void delete_client(client* client){
    client->in_use = 0;
    client->name[0] = 0;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);
    printf("Deleted a client\n");
}

client* create_client(int fd, char* name, struct sockaddr *addr){
    pthread_mutex_lock(&mutex);
    int i = 0;
    while (i<MAX_CLIENTS && clients[i].in_use == 1) i++;
    if (i>=MAX_CLIENTS) return NULL;
    client* client = &clients[i];

    client->fd = fd;
    client->in_use = 1;
    client->online = 1;
    client->ready = 0;
    client->addr = addr;
    strcpy(client->name, name);

    pthread_mutex_unlock(&mutex);
    return client;
}
