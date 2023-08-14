#include <stdio.h>
#include <stdlib.h>
#include "task.h"

Task *tasks[5];
int task_count = 0;

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
}

// invoke the scheduler
void schedule()
{
    printf("Attempting to schedule %d tasks\n", task_count);

    for (int i = 0; i < task_count; i++)
    {
        printf("Scheduling task %s\n", tasks[i]->name);
    }
}
