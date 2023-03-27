
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>


int calls=-1;
int mode ;
void handler(int signum, siginfo_t *info, void *context){
	calls++;

	mode = info->si_value.sival_int;
	printf("\nreceived mode %d\n", mode);

	union sigval val;
	val.sival_int = 1;
	sigqueue(info->si_pid, SIGUSR1, val);
}

int run(){
	if (mode ==0){

	} else if (mode == 1){
		printf("  ");
		for (int i=1;i<=10;i++) 
			printf("%d ", i);
		printf("\n");
	} else if (mode == 2){
		time_t current_time = time(0);
		printf("  %ld\n", current_time);
	} else if (mode == 3){
		printf("  calls since start of a program: %d (initial command not included)\n", calls);
	} else if (mode == 4){
		time_t current_time = time(0)-1;
		printf("  %ld\n", current_time);
		while (mode == 4){
			sleep(1);
			printf("  %ld\n", time(0));
		}
		run();
	} else if (mode==5){
		exit(1);
	}
	else {
		printf(   "WRONG MODE NUMBER\n");
	}

	return 1;
}

int main(){



	printf("PID: %d\n", getpid());


	struct sigaction act;

	act.sa_sigaction = handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGUSR1, &act, NULL);


	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGUSR1);

	sigprocmask(SIG_SETMASK, &set, NULL);

	while(mode != 5){

		

		int f = run();

		if (!f) return 0;
		sigsuspend(&set);

	}

}