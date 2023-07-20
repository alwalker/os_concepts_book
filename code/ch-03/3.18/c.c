#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
pid_t pid;
	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Fork Failed"); 
		return 1;
	}
	else if (pid == 0) {
		printf("Doing child things...\n");
        // for(;;){}
	}
	else {
        printf("Parrent infinite loop starting...\n");
        for(;;){}
        printf("Parent waiting...\n");
		wait(NULL);
		printf("Finished wait, exiting...\n");
	}

	return 0;
}
