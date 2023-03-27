#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int sent[6] = {1,0,0,0,0,0};

void handler(int signum, siginfo_t *info, void *context){
	printf(" confirmed..\n");
}

int main(int argc, char** argv){

	int pid = atoi(argv[1]);


	struct sigaction act;

	act.sa_sigaction = &handler;

	sigaction(SIGUSR1, &act, NULL);
	


	union sigval val;
	val.sival_int = 0;

	sigqueue(pid, SIGUSR1, val);


	char input[5];

	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGUSR1);

	while (1){

	  // wait for confirmation
		sigsuspend(&set);
      
		sigaction(SIGUSR1, &act, NULL);
	
	  // get mode to send
		fgets(input, 5, stdin);
		val.sival_int = atoi(input);
		sent[atoi(input)] = 1;

		printf("sent %d\n", val.sival_int);

		sigqueue(pid, SIGUSR1, val);

		int flag = 0;
		for (int i=1;i<=5;i++){
			if (sent[i] == 0) {
				flag = 1;
				break;
			}
		}
		if (flag==0) return 0;


	}
}