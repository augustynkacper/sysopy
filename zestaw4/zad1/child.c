#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv){

	if (argc!=2){
		printf("Program accepts exactly 1 argument!\n");
		return 0;
	}


	if ( strcmp(argv[1], "ignore")==0 ){
		printf("raising signal... [child process]\n");
		raise(SIGUSR1);
		
	} else if ( strcmp(argv[1], "mask")==0 ){
		printf("raising signal... [child process]\n");
		raise(SIGUSR1);
	} else if ( strcmp(argv[1], "pending")==0 ){
		sigset_t set;

		sigpending( &set );
		if ( sigismember(&set, SIGUSR1)==1 )
			printf("SIGUSR1 signal is pending... [child process]\n");
		else
			printf("SIGUSR1 signal is not pending... [child process]\n");
	}


}