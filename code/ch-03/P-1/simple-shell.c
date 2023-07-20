#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80
#define READ_END 0
#define WRITE_END 1

int main(void)
{
	FILE *redirect_file = NULL;
	char *args[MAX_LINE / 2 + 1];
	char input[MAX_LINE];
	char *history = NULL;
	char *token;
	char *delim = " ";
	int arg_index = 0;
	int exec_ret;
	int pipe_index = 0;
	int should_run = 1;

	while (should_run)
	{
		memset(input, 0, MAX_LINE * sizeof(char));
		for (int i = 0; i < (MAX_LINE / 2 + 1); i++)
		{
			args[i] = NULL;
		}

		printf("osh>");
		fflush(stdout);

		fgets(input, MAX_LINE, stdin);
		input[strcspn(input, "\n")] = 0;
		printf("Got input: %s\n", input);

		if (strcmp(input, "exit") == 0)
		{
			break;
		}

		if (strcmp(input, "!!") == 0)
		{
			if (history == NULL)
			{
				fprintf(stderr, "No commands in history.\n");
				history = (char *)calloc(MAX_LINE, sizeof(char));
				continue;
			}
			else
			{
				strcpy(input, history);
				printf("%s\n", history);
			}
		}
		else
		{
			if (history == NULL)
			{
				history = (char *)calloc(MAX_LINE, sizeof(char));
			}
			strcpy(history, input);
		}

		token = strtok(input, delim);
		while (token)
		{
			if (token[0] == '|')
			{
				pipe_index = arg_index;
			}

			args[arg_index] = token;
			arg_index++;
			token = strtok(NULL, delim);
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
			if (pipe_index != 0)
			{
				printf("WE GOT PIPES\n");
				fflush(stdout);

				int fd[2];
				if (pipe(fd) == -1)
				{
					fprintf(stderr, "Pipe failed\n");
					exit(1);
				}

				pid_t right_of_pipe_pid;
				right_of_pipe_pid = fork();
				if (right_of_pipe_pid < 0)
				{
					fprintf(stderr, "Child Fork Failed\n");
					exit(1);
				}
				else if (right_of_pipe_pid == 0)
				{
					printf("Inner child executing %s on right hand of pipe...\n", args[pipe_index + 1]);
					fflush(stdout);

					// char fuckyou[1000];
					// read(STDIN_FILENO, fuckyou, 1000);
					// printf("Inner child got: %s", fuckyou);
					// fflush(stdout);
					// _Exit(0);

					dup2(fd[READ_END], STDIN_FILENO);
					dup2(fd[WRITE_END], STDOUT_FILENO);

					int inner_exec_ret = execvp(args[pipe_index + 1], &args[pipe_index + 1]);
					if (inner_exec_ret != 0)
					{
						printf("Right hand of pipe failed\n");
						fflush(stdout);
						exit(exec_ret);
					}
				}
				else
				{
					char *left_args[MAX_LINE / 2 + 1];
					for (int i = 0; i < pipe_index; i++)
					{
						left_args[i] = args[i];
					}

					printf("Inner parent executing %s on left hand of pipe...\n", left_args[0]);
					fflush(stdout);

					dup2(fd[WRITE_END], STDOUT_FILENO);
					dup2(fd[READ_END], STDIN_FILENO);

					exec_ret = execvp(left_args[0], left_args);
					if (exec_ret != 0)
					{
						exit(exec_ret);
					}
				}
			}
			else
			{
				if (strcmp(args[arg_index - 1], "&") == 0)
				{
					args[arg_index - 1] = NULL;
				}

				if (arg_index >= 3 && *args[arg_index - 2] == '>')
				{
					redirect_file = fopen(args[arg_index - 1], "w");
					args[arg_index - 1] = NULL;
					args[arg_index - 2] = NULL;
					dup2(redirect_file->_fileno, STDOUT_FILENO);
				}
				else if (arg_index >= 3 && *args[arg_index - 2] == '<')
				{
					redirect_file = fopen(args[arg_index - 1], "r");
					args[arg_index - 1] = NULL;
					args[arg_index - 2] = NULL;
					dup2(redirect_file->_fileno, STDIN_FILENO);
				}

				exec_ret = execvp(args[0], args);

				if (exec_ret != 0)
				{
					exit(exec_ret);
				}
			}
		}
		else
		{
			if (strcmp(args[arg_index - 1], "&") != 0)
			{
				wait(NULL);
			}

			arg_index = 0;
			pipe_index = 0;
			if (redirect_file != NULL)
			{
				fclose(redirect_file);
				redirect_file = NULL;
			}
		}
	}

	return 0;
}
