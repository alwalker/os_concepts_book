#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Invalid number of arguments\n");
		return 1;
	}
	int input = atoi(argv[1]);
	if (input <= 0)
	{
		fprintf(stderr, "Non positive number given\n");
		return 1;
	}

	int fd[2];
	if (pipe(fd) == -1)
	{
		fprintf(stderr, "Pipe failed\n");
		return 1;
	}

	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "Fork Failed\n");
		return 1;
	}
	else if (pid == 0)
	{
		char *collatz_sequence = (char *)calloc(1024, sizeof(char));
		char *str_input = (char *)calloc(32, sizeof(char));

		printf("CHILD: Generating Collatz sequence...\n");
		for (;;)
		{
			sprintf(str_input, "%d, ", input);
			strcat(collatz_sequence, str_input);

			if (input == 1)
			{
				collatz_sequence[strlen(collatz_sequence) - 2] = '\0';
				break;
			}
			else if (input % 2 == 0)
			{
				input = input / 2;
			}
			else
			{
				input = (input * 3) - 1;
			}
		}

		printf("CHILD: Found sequence: %s\n", collatz_sequence);

		printf("CHILD: Writing to pipe...\n");
		close(fd[READ_END]);
		write(fd[WRITE_END], collatz_sequence, strlen(collatz_sequence));
		close(fd[WRITE_END]);
		free(collatz_sequence);
		free(str_input);
	}
	else
	{
		printf("PARENT: waiting...\n");
		wait(NULL);

		printf("PARENT: Reading from pipe...\n");
		char *collatz_sequence = (char *)calloc(1024, sizeof(char));
		close(fd[WRITE_END]);
		read(fd[READ_END], collatz_sequence, 1024*sizeof(char));
		close(fd[READ_END]);

		printf("PARENT: Collatz Sequence: %s\n", collatz_sequence);
	}

	return 0;
}
