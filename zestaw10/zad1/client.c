#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

#define MAX_MESSAGE_LENGTH 64

int socket_id;

void handler(int){
    send(socket_id, "STOP", MAX_MESSAGE_LENGTH, 0);
    printf("left\n");
    exit(1);
}

int web_connection(int port);
int local_connection(char * path);

void send_message(char* msg, int fd){
    send(fd, msg, MAX_MESSAGE_LENGTH, 0);
}

int main(int argc, char** argv){
    signal(SIGINT, handler);

    if (strcmp(argv[2], "unix") ==0)
        socket_id = local_connection(argv[3]);
    else if (strcmp(argv[2], "web") == 0)
        socket_id = web_connection(atoi(argv[3]));
    char* name = argv[1];

    send_message(name, socket_id);

    int epoll_fd = epoll_create1(0);
    struct epoll_event stdin_event = {
            .events = EPOLLIN | EPOLLPRI,
            .data = { .fd = STDIN_FILENO }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

    struct epoll_event socket_event = {
            .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
            .data = { .fd = socket_id }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_id, &socket_event);

    struct epoll_event events[2];
    while(1){
        int events_occured = epoll_wait(epoll_fd, events, 2, -1);
        for (int i=0; i<events_occured; i++) {
            if (events[i].data.fd == STDIN_FILENO){
                char buffer[256] = "";
                int x = read(STDIN_FILENO, &buffer, 256);
                buffer[x-1] = 0;
                send_message(buffer, socket_id);
                if (strcmp(buffer, "STOP")==0) exit(1);
                //send(socket_id, buffer, sizeof(buffer), 0);
            } else {
                char buffer[256];
                long n = recv(socket_id, buffer, 256, 0);
                buffer[n] = 0;
                //puts(buffer);
                if (strcmp(buffer, "STOP")==0 ){
                    exit(1);
                } else if (strcmp(buffer, "PING")==0){
                    send_message("PING", socket_id);
                    //send(socket_id, "PING", sizeof(buffer), 0);
                } else {
                    puts(buffer);
                }
            }

        }
    }
}

int web_connection(int port){
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    struct hostent *server = gethostbyname("127.0.0.1");
    addr.sin_addr.s_addr = *((unsigned long*)server->h_addr);

    int web_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_socket < 0){
        puts("socket error");
    }
    connect(web_socket, (struct sockaddr*) &addr, sizeof addr);

    return web_socket;
}
int local_connection(char * path){
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof addr.sun_path);

    int local_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    connect(local_socket, (struct sockaddr*) &addr, sizeof addr);

    return local_socket;
}