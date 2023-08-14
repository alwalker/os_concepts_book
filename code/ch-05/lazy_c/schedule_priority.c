#include <stdio.h>
#include <stdlib.h>
#include "task.h"

int total_task_count = 0;
int task_counts[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Task ***tasks = NULL;

// add a task to the list
void add(char *name, int priority, int burst)
{
    if (tasks == NULL)
    {
        printf("Initialize two dimensional task array\n");
        tasks = (Task ***)calloc(10, sizeof(Task **));

        for (int i = 0; i < 5; i++)
        {
            tasks[i] = (Task **)calloc(5, sizeof(Task *));
        }
    }

    printf("Adding task %s with priority %d and CPU burst %d\n", name, priority, burst);

    tasks[priority][task_counts[priority]] = (Task *)calloc(1, sizeof(Task));

    tasks[priority][task_counts[priority]]->name = name;
    tasks[priority][task_counts[priority]]->tid = total_task_count;
    tasks[priority][task_counts[priority]]->priority = priority;
    tasks[priority][task_counts[priority]]->burst = burst;

    task_counts[priority] = task_counts[priority] + 1;
    total_task_count++;
}

// invoke the scheduler
void schedule()
{
    printf("Attempting to schedule %d tasks\n", total_task_count);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < task_counts[i]; j++)
        {
            printf("Scheduling task %s\n", tasks[i][j]->name);
        }
    }
}
