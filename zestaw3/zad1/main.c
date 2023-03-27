

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>



int main(int argc, char* argv[]){
    if (argc!=2){
        printf("program accepts exactly 1 argument!\n");
        return 0;
    }

    int no_proc = atoi(argv[1]);

    if (no_proc == 0){
        printf("pass correct argument!\n");
        return 0;
    }


    int pid;

    for (int i=0; i<no_proc; i++){

        pid = fork();

        if (pid == 0){
            printf("parent:%d current:%d\n", getppid(), getpid());
            return 0;
        }
    }
    while (wait(0) > 0);
    printf("number of processes: %d\n", no_proc);

    return 0;
}