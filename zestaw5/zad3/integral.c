
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>


#define BUFF_SIZE 128

double f(double a, double b, double width){
    double res = 0.0;

    while (a<b){
        
        if (a+width>b){
            res += (b-a)*4/(a*a+1); 
        } else {
            res += width*4/(a*a+1);
        }
        a += width;
    }

    return res;
}

// first argument - width of rectangle
// second argument - x
int main(int argc, char** argv){
    double a = strtod(argv[1], NULL);
    double b = strtod(argv[2], NULL);
    double width = strtod(argv[3], NULL);

    double value = f(a, b, width);
    char buff[BUFF_SIZE];
    snprintf(buff, BUFF_SIZE, "%f", value);

    //printf("%Lf %s\n", x,buff);
    int fifo = open("pipe", O_WRONLY);
    write(fifo, buff, BUFF_SIZE);
    close(fifo);


    return 0;

}