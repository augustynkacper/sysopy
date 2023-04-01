
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define BUFF_SIZE 128
char arg_a[BUFF_SIZE], arg_b[BUFF_SIZE];

int main(int argc, char** argv){

    // structures for counting the execution time
    struct timespec start, end, elapsed;
    clock_gettime(CLOCK_REALTIME, &start);

    // get rectangle width and number of child processes
    double width = strtold(argv[1], NULL);
    int n = atoi(argv[2]);


    double range_length = 1.0/n;
    double a = 0.0;

    // create a named pipe
    mkfifo("pipe", 0666);

    // start n programs to count integrals on given range
    for (int j=0; j<n; j++){
        if (!fork()) {
            snprintf(arg_a, BUFF_SIZE, "%f", a);
            snprintf(arg_b, BUFF_SIZE, "%f", a+range_length);
            execl("./integral", "integral", arg_a, arg_b, argv[1], NULL);
            return 0;
        }
        a += range_length;
    }

    // after integral counted, open a pipe
    int fifo = open("pipe", O_RDONLY);
    int read_parts =0;
    char buf[BUFF_SIZE];

    double result = 0.0;
    size_t bytes_read;

    // read from pipe and count result
    while (read_parts<n){

        bytes_read = read(fifo, buf, BUFF_SIZE);
        buf[bytes_read] = 0;

        char* token;
        token = strtok(buf, "\n");

        while (token){
            result += strtod(token, NULL);
            read_parts++;
            token = strtok(NULL, "\n");
        }
    }

    // count execution time
    clock_gettime(CLOCK_REALTIME, &end);
    elapsed.tv_sec = end.tv_sec - start.tv_sec;
    elapsed.tv_nsec = end.tv_nsec - start.tv_nsec;
    if (elapsed.tv_nsec < 0){
        elapsed.tv_sec --;
        elapsed.tv_nsec += 1000000000L;
    }

    // print results
    printf("no_children: %d\n", n);
    printf("rect_width: %.20lf\n", width);
    printf("  result: %f\n", result);    
    printf("  execution time: %ld.%05lds\n", elapsed.tv_sec, elapsed.tv_nsec);

    return 0;
}