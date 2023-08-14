#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "cpu.h"

Task *tasks[6];
int task_count = 0;
int total_cpu_time = 0;

// add a task to the list
void add(char *name, int priority, int burst)
{
    printf("Adding task %s with priority %d and CPU burst %d\n", name, priority, burst);

    tasks[task_count] = (Task *)calloc(1, sizeof(Task));

    tasks[task_count]->name = name;
    tasks[task_count]->tid = task_count;
    tasks[task_count]->priority = priority;
    tasks[task_count]->burst = burst;

    task_count++;
    total_cpu_time = total_cpu_time + burst;
}

// invoke the scheduler
void schedule()
{
    printf("Attempting to schedule %d tasks\n", task_count);
    fflush(stdout);

    int i = 0;
    while (total_cpu_time > 0)
    {
        printf("Round robin'ing on process %s with %d time left and %d total time left\n", tasks[i]->name, tasks[i]->burst, total_cpu_time);
        fflush(stdout);
        total_cpu_time = total_cpu_time - (QUANTUM < tasks[i]->burst ? QUANTUM : tasks[i]->burst);

        if (tasks[i]->burst > 0)
        {
            printf("Scheduling task %s\n", tasks[i]->name);
            fflush(stdout);
            tasks[i]->burst = tasks[i]->burst - QUANTUM;
        }

        i = (i + 1) % 6;
    }
}
