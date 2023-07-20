#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	const char *SHM_NAME = "OS319";
	const int SHM_SIZE = 4096;

	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid == 0)
	{
		printf("CHILD: Setting up shm...\n");
		int shm_fd;
		void *ptr;
		shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
		ftruncate(shm_fd, SHM_SIZE);
		ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if (ptr == MAP_FAILED)
		{
			printf("CHILD: Map failed\n");
			return -1;
		}

		printf("CHILD: Writing to shm...\n");
		struct timeval start;
		gettimeofday(&start, NULL);
		printf("CHILD: Start time was %d\n", start.tv_usec);
		memcpy(ptr, &start, sizeof(start));

		printf("CHILD: Running command...\n");
		execv(argv[1], argv + 2);
	}
	else
	{
		printf("PARENT: waiting...\n");
		wait(NULL);

		printf("PARENT: Reading from shm...\n");
		int shm_fd;
		void *ptr;
		shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
		ftruncate(shm_fd, SHM_SIZE);
		ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if (ptr == MAP_FAILED)
		{
			printf("PARENT: Map failed\n");
			return -1;
		}

		struct timeval start, end;
		gettimeofday(&end, NULL);
		memcpy(&start, ptr, sizeof(start));
		printf("PARENT: Start time was: %d\n", start.tv_usec);
		printf("PARENT: End time was: %d\n", end.tv_usec);
		printf("PARENT: Total time was: %d\n", end.tv_usec - start.tv_usec);
	}

	return 0;
}
