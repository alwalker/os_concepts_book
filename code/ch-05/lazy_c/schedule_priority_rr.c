#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "cpu.h"

int total_task_count = 0;
int total_cpu_time = 0;
int task_counts[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int task_times[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Task ***tasks = NULL;

// add a task to the list
void add(char *name, int priority, int burst)
{
    printf("Adding task %s with priority %d and CPU burst %d\n", name, priority, burst);

    if (tasks == NULL)
    {
        printf("Initialize two dimensional task array\n");
        tasks = (Task ***)calloc(10, sizeof(Task **));

        for (int i = 0; i < 6; i++)
        {
            tasks[i] = (Task **)calloc(6, sizeof(Task *));
        }
    }

    tasks[priority][task_counts[priority]] = (Task *)calloc(1, sizeof(Task));

    tasks[priority][task_counts[priority]]->name = name;
    tasks[priority][task_counts[priority]]->tid = total_task_count;
    tasks[priority][task_counts[priority]]->priority = priority;
    tasks[priority][task_counts[priority]]->burst = burst;

    task_counts[priority] = task_counts[priority] + 1;
    task_times[priority] = task_times[priority] + burst;

    total_task_count++;
    total_cpu_time = total_cpu_time + burst;
}

// invoke the scheduler
void schedule()
{
    printf("Attempting to schedule %d tasks\n", total_task_count);
    fflush(stdout);

    for (int i = 0; i < 10; i++)
    {
        int j = 0;
        while (task_times[i] > 0)
        {
            printf(
                "Round robin'ing on process %s with %d time left out of %d task time and %d total time left\n",
                tasks[i][j]->name,
                tasks[i][j]->burst,
                task_times[i],
                total_cpu_time);

            total_cpu_time = total_cpu_time - (QUANTUM < tasks[i][j]->burst ? QUANTUM : tasks[i][j]->burst);
            task_times[i] = task_times[i] - (QUANTUM < tasks[i][j]->burst ? QUANTUM : tasks[i][j]->burst);

            if (tasks[i][j]->burst > 0)
            {
                printf("Scheduling task %s\n", tasks[i][j]->name);
                tasks[i][j]->burst = tasks[i][j]->burst - QUANTUM;
            }

            j = (j + 1) % task_counts[i];
        }
    }
}
