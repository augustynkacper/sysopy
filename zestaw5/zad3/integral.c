
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>


#define BUFF_SIZE 128

long double f(long double x){
    return 4/(x*x+1);
}

// first argument - width of rectangle
// second argument - x
int main(int argc, char** argv){
    long double width = strtod(argv[1], NULL);
    long double x = strtod(argv[2], NULL);

    long double value = f(x)*width;
    char buff[BUFF_SIZE];
    snprintf(buff, BUFF_SIZE, "%Lf", value);

    //printf("%Lf %s\n", x,buff);
    int fifo = open("pipe", O_WRONLY);
    write(fifo, buff, BUFF_SIZE);
    close(fifo);


    return 0;

}