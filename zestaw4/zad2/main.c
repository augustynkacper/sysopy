
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


void resethand_handler(int signum){
	printf("signal %d caught\n", signum);
}


void siginfo_handler(int signum, siginfo_t *info, void *context){
	printf("handling signal %d:\n", signum);
	printf("   sending process ID: %d\n", info->si_pid);
	printf("   user ID sending process id: %d\n", info->si_uid);
	printf("   sent value: %d\n", info->si_value.sival_int);
}

void nocldstop_handler(int signum){
	printf("SIGCHLD handled\n");
}

int main(){


// SA_NOCLDSTOP
	printf("--- SA_NOCLDSTOP test ---\n");
	struct sigaction sa3;

	sa3.sa_handler = &nocldstop_handler;
	
	if ( sigaction(SIGCHLD, &sa3, NULL)<0 ){
		printf("couldnt catch SIGCHLD");
	}

	
	sa3.sa_flags = SA_NOCLDSTOP; fflush(stdout);

	pid_t pid = fork();
	if (pid==0){
		while(1);
	} else if (pid > 0){
		kill(pid, SIGSTOP);
	}

	
// SA_SIGINFO
	printf("\n--- SA_SIGINFO test ---\n");
	struct sigaction sa2;
	sa2.sa_sigaction = siginfo_handler;
	sa2.sa_flags = SA_SIGINFO;

	if ( sigaction(SIGUSR2, &sa2, NULL) < 0 ){
		printf("couldnt handle SIGUSR1\n");  exit(1);
	}

	union sigval val;
	val.sival_int = 90210;

	if ( sigqueue(getpid(), SIGUSR2, val)<0 ){
		printf("couldnt send SIGUSR1\n");  exit(1);
	}


// SA_RESETHAND
	printf("\n--- SA_RESETHAND test ---\n");
	struct sigaction sa1;
	sa1.sa_handler = resethand_handler;
	sa1.sa_flags = SA_RESETHAND;

	if ( sigaction(SIGUSR1, &sa1, NULL) < 0 ){
		printf("couldnt handle SIGUSR1\n");  exit(1);
	}

	raise(SIGUSR1);
	raise(SIGUSR1);

	printf("after signal\n");


}