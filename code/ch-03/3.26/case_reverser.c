#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Invalid number of arguments\n");
		return 1;
	}

	int child_to_parent_pipe[2];
	if (pipe(child_to_parent_pipe) == -1)
	{
		fprintf(stderr, "Write Pipe failed\n");
		return 1;
	}
	int parent_to_child_pipe[2];
	if (pipe(parent_to_child_pipe) == -1)
	{
		fprintf(stderr, "Read Pipe failed\n");
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
		printf("CHILD: Reading from read pipe...\n");
		char *input = (char *)calloc(strlen(argv[1]), sizeof(char));
		close(parent_to_child_pipe[WRITE_END]);
		read(parent_to_child_pipe[READ_END], input, strlen(argv[1]) * sizeof(char));
		close(parent_to_child_pipe[READ_END]);
		printf("CHILD: Got %s\n", input);

		char *output = (char *)calloc(strlen(argv[1]), sizeof(char));
		for (int i = 0; i < strlen(input); i++)
		{
			if (islower(input[i]))
			{
				output[i] = toupper(input[i]);
			}
			else
			{
				output[i] = tolower(input[i]);
			}
		}

		printf("CHILD: Reversed input: %s\n", output);

		printf("CHILD: Writing to write pipe...\n");
		close(child_to_parent_pipe[READ_END]);
		write(child_to_parent_pipe[WRITE_END], output, strlen(output) * sizeof(char));
		close(child_to_parent_pipe[WRITE_END]);
		free(output);
		free(input);
	}
	else
	{
		printf("PARENT: Writing to read pipe...\n");
		close(parent_to_child_pipe[READ_END]);
		write(parent_to_child_pipe[WRITE_END], argv[1], strlen(argv[1]) * sizeof(char));
		close(parent_to_child_pipe[WRITE_END]);

		printf("PARENT: waiting...\n");
		wait(NULL);

		printf("PARENT: Reading from write pipe...\n");
		char *input = (char *)calloc(strlen(argv[1]), sizeof(char));
		close(child_to_parent_pipe[WRITE_END]);
		read(child_to_parent_pipe[READ_END], input, strlen(argv[1]) * sizeof(char));
		close(child_to_parent_pipe[READ_END]);
		printf("PARENT: Got %s\n", input);

		free(input);
	}

	return 0;
}
