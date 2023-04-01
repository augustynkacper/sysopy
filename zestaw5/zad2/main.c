#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define BUFF_SIZE 128

char read_buf[BUFF_SIZE];

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

int main(int argc, char** argv){

    // structures for counting the execution time
    struct timespec start, end, elapsed;
    clock_gettime(CLOCK_REALTIME, &start);

    // get rectangle width and number of children
    double width = strtold(argv[1], NULL);
    int n = atoi(argv[2]);

   
    double range_length = 1.0/n;

    // initialize pipe array
    int fd[n][2];

    int w;
    double value;   // area of single rectangle
    char char_num[BUFF_SIZE];  // used for filling pipe array 

    double a = 0.0;
    
    // start n child processes to count integrals on given range
    for (int i=0; i<n; i++){
        pipe(fd[i]);
        
        pid_t pid = fork();

        if (pid == 0){
            close(fd[i][0]);
                
            value = f(a, a+range_length, width);

            snprintf(char_num, BUFF_SIZE, "%f", value);
            w = write(fd[i][1], char_num, BUFF_SIZE);
            return 0;
        } 
        close(fd[i][1]);
        a+=range_length;
    }

    while(wait(NULL)>0);
    

    // reading from pipe and counting the integral
    double result = 0.0;
    for (int i=0; i<n; i++){
        w = read(fd[i][0], read_buf, BUFF_SIZE);
        read_buf[w] = 0;
        result += strtod(read_buf, NULL);
    }

    // count the execution time
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