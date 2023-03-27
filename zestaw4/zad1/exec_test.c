#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int mask_and_pending(char * arg){
	sigset_t new_mask, set;

 // masking SIGUSR1 signal

	sigemptyset( &new_mask );

	sigaddset( &new_mask, SIGUSR1 );

	if ( sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0 ) 
		printf("Couldn't block a signal!");

	 // sending signal
 	printf("raising signal... [main process]\n");
	raise(SIGUSR1);
 
 // checking if signal is pending
 	sigpending( &set );

 	if ( sigismember(&set, SIGUSR1)==1 )
 		printf("SIGUSR1 signal is pending... [main process]\n");
 	else
 		printf("SIGUSR1 signal is not pending... [main process]\n");




	printf("running child process...\n");
	if ( execl("./child", "./child", arg, NULL) == -1){
		printf("Couldn't start child program...\n");
		return -1;
	}

	return 1;
}

int main(int argc, char** argv){

	if (argc!=2){
		printf("Program accepts exactly 1 argument!\n");
		return 0;
	}


	if ( strcmp(argv[1], "ignore")==0 ){
		signal(SIGUSR1, SIG_IGN);

		printf("raising signal... [main process]\n");
		raise(SIGUSR1);

		if ( execl("./child", "./child", argv[1], NULL) == -1){
			printf("Couldn't start child program...\n");
			return 0;
		}
		
	} else if ( strcmp(argv[1], "mask")==0 ){
		
		mask_and_pending(argv[1]);
	} else if ( strcmp(argv[1], "pending")==0 ){

		mask_and_pending(argv[1]);

	}


	





}