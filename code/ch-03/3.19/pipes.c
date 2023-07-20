#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{
	int fd[2];

	if (pipe(fd) == -1)
	{
		fprintf(stderr, "Pipe failed");
		return 1;
	}

	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0)
	{

		struct timeval start;
		gettimeofday(&start, NULL);
		printf("CHILD: Start time was %d\n", start.tv_usec);

		printf("CHILD: Writing to pipe...\n");
		close(fd[READ_END]);
		write(fd[WRITE_END], &start.tv_usec, sizeof(start.tv_usec));
		close(fd[WRITE_END]);

		printf("CHILD: Running command...\n");
		execv(argv[1], argv + 2);
	}
	else
	{
		printf("PARENT: waiting...\n");
		wait(NULL);

		printf("PARENT: Reading from pipe...\n");
		struct timeval start, end;
		gettimeofday(&end, NULL);

		close(fd[WRITE_END]);
		read(fd[READ_END], &start.tv_usec, sizeof(start.tv_usec));
		close(fd[READ_END]);

		printf("PARENT: Start time was: %d\n", start.tv_usec);
		printf("PARENT: End time was: %d\n", end.tv_usec);
		printf("PARENT: Total time was: %d\n", end.tv_usec - start.tv_usec);
	}

	return 0;
}
