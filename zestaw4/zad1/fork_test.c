#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


void handler(){
	printf("Handler for SIGUSR1\n");
}

void make_fork(){
	printf("raising signal... [main process]\n");
	raise(SIGUSR1);

	pid_t pid = fork();
	if ( pid==0 ){
		printf("raising signal... [child process]\n");
		raise(SIGUSR1);
	}
	wait(NULL);
}

void mask_and_pending(sigset_t new_mask, sigset_t set){
 // masking SIGUSR1 signal
	sigemptyset( &new_mask );

	sigaddset( &new_mask, SIGUSR1 );

	if ( sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0 ) 
		printf("Couldn't block a signal!");

 // sending signal
 	raise( SIGUSR1 );

 // checking if signal is pending
 	sigpending( &set );

 	if ( sigismember(&set, SIGUSR1)==1 )
 		printf("SIGUSR1 signal is pending... [main process]\n");
 	else
 		printf("SIGUSR1 signal is not pending... [main process]\n");
}

int main(int argc, char** argv){

	if ( argc != 2){
		printf("Program accepts exactly one argument!\n");
		return 0;
	}


	if ( strcmp(argv[1], "ignore")==0 ){
		signal(SIGUSR1, SIG_IGN);
		make_fork();
	} else if ( strcmp(argv[1], "handler")==0 ){
		signal(SIGUSR1, handler);
		make_fork();
	} else if ( strcmp(argv[1], "mask")==0 ){
	 	sigset_t new_mask, set;
		mask_and_pending(new_mask, set);
	 // fork
	 	make_fork();
	} else if ( strcmp(argv[1], "pending")==0 ){
		sigset_t new_mask, set;
		mask_and_pending(new_mask, set);

		pid_t pid = fork();

		if (pid!=0){
			while(1);
		}
		else if ( pid== 0){
			sigpending( &set );
	
			if ( sigismember(&set, SIGUSR1)==1 )
			 	printf("SIGUSR1 signal is pending... [child process]\n");
			else
			 	printf("SIGUSR1 signal is not pending... [child process]\n");

			exit(1);
		} 
		
		
		
	}


	


	
	

}
