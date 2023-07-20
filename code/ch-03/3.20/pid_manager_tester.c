#include <stdio.h>
#include "pid_manager.h"

int main(void)
{
    allocate_map();

    int pid = allocate_pid();
    printf("First pid is %d\n", pid);

    for (int i = 0; i < 9; i++)
    {
        pid = allocate_pid();
    }
    printf("10th pid is %d\n", pid);

    release_pid(305);
    pid = allocate_pid();
    printf("Pid after reallocation: %d\n", pid);

    int prev_pid;
    for (int i = 0; i < 10000; i++)
    {
        prev_pid = pid;
        pid = allocate_pid();

        if (pid == -1)
        {
            printf("Stopped allocating pids at: %d\n\n", prev_pid);
            print_map();
            printf("\n");
            break;
        }
    }

    printf("Releasing some pids...\n");
    release_pid(409);
    release_pid(1000);
    release_pid(3999);

    pid = allocate_pid();
    printf("Allocated another pid, should be 409: %d\n", pid);
    pid = allocate_pid();
    printf("Allocated another pid, should be 1000: %d\n", pid);

    return 0;
}