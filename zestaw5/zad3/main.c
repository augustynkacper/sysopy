
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


char arg_x[124];

int main(int argc, char** argv){


    // get rectangle width
    long double width;
    width = strtold(argv[1], NULL);

    // create pipe array of rectangles_number length
    int n = (int)(1/width);

    printf("n: %d, width: %Lf\n", n, width);

    long double x = 0.0;

    mkfifo("pipe", 0666);

    for (int j=0; j<n; j++){
        pid_t pid ;

        if (!fork()) {
            snprintf(arg_x, 124, "%Lf", x);
            execl("./integral", "integral", argv[1], arg_x, NULL);
            return 0;
        }

        x += width;
        
    }


    int fifo = open("pipe", O_RDONLY);
    int already_read =0;

    char buf[128];
    long double result = 0.0;
    while (already_read<n){
        size_t size = read(fifo, buf, 128);
        buf[size] = 0;


        char* token;

        token = strtok(buf, "\n");
        while (token){
            result += strtod(token, NULL);
            already_read++;
            token = strtok(NULL, "\n");
        }
       
    }
    printf("result = %Lf\n", result);

    return 0;
}