


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char* argv[]) {

    if (argc != 2){
        printf("program accepts exactly 1 argument!\n");
        return 0;
    }

    printf("%s ", argv[0]);   fflush(stdout);


    execl("/bin/ls", "/bin/ls", argv[1]);


    return 0;
}