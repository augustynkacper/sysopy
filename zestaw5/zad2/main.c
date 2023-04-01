#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

char read_buf[50];

long double f(long double x, long double width){
    return width*4/(x*x+1);
}

int main(int argc, char** argv){


    // get rectangle width
    long double width;
    width = strtold(argv[1], NULL);

    // create pipe array of rectangles_number length
    int pipe_length = (int)(1/width);
    int fd[pipe_length][2];

    
    int i=0, w;
    long double value;   // area of single rectangle
    char char_num[100];  // used for filling pipe array 


    
    for (long double a=0; a<=1; a+=width){
        pipe(fd[i]);

        pid_t pid = fork();

        if (pid == 0){
            close(fd[i][0]);
            
            value = f(a, width);
            snprintf(char_num, 100, "%Lf", value);
           
            w = write(fd[i][1], char_num, 50);
         
            return 0;
        } 
        close(fd[i][1]);
        i++;
    }

    while(wait(NULL)>0);
    
    long double result = 0.0;

    for (i=0; i<pipe_length; i++){
        w = read(fd[i][0], read_buf, 50);
        read_buf[w] = 0;
        result += strtod(read_buf, NULL);
    }

    printf("%Lf\n", result);
    return 0;
}