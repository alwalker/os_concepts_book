#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_SIZE 3

typedef struct
{
    char *pid;
    unsigned long start_address;
    unsigned long stop_address;
} memory_chunk;

void parse_line(char *, char *, char *, unsigned long *, char *);
void insert_process(char *, unsigned long, int);
void request_memory(char *, unsigned long, char);
void release_memory(char *);
void remove_process(int, int);
void compact();
void stat();

int chunk_count = 0;
unsigned long address_space = 0;
memory_chunk **memory_map;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Please supply total address space.\n");
        return 1;
    }

    address_space = strtoul(argv[1], NULL, 0);

    memory_map = (memory_chunk **)calloc(1024, sizeof(memory_chunk *));
    memory_map[0] = (memory_chunk *)malloc(sizeof(memory_chunk));
    memory_map[0]->pid = "HOLE";
    memory_map[0]->start_address = 0;
    memory_map[0]->stop_address = address_space - 1;
    chunk_count = 1;

    size_t line_size;
    char *line = NULL;
    char *command = (char *)calloc(COMMAND_SIZE, sizeof(char));
    char *pid = (char *)calloc(6, sizeof(char));
    unsigned long size = 0;
    char algorithm;

    for (;;)
    {
        printf("allocator> ");
        getline(&line, &line_size, stdin);
        printf("\n");

        parse_line(line, command, pid, &size, &algorithm);

        // printf("Parsed\nCommand: %s\nPID: %s\nSize: %lu\nAlgorithm: %c\n", command, pid, size, algorithm);

        if (strncmp(command, "RQ", 3) == 0)
        {
            request_memory(pid, size, algorithm);
        }
        else if (strncmp(command, "RL", 3) == 0)
        {
            release_memory(pid);
        }
        else if (strncmp(command, "CP", 3) == 0)
        {
            compact();
        }
        else if (strncmp(command, "ST", 3) == 0)
        {
            stat();
        }
        else if (strncmp(command, "EX", 3) == 0)
        {
            exit(0);
        }

        size = 0;
        algorithm = '\0';
        command[0] = '\0';
        pid[0] = '\0';
        free(line);
        line = NULL;
    }
}

void compact()
{
    int new_chunk_count = 0;
    int new_start_address = 0;

    memory_chunk **old = memory_map;
    memory_chunk **temp = (memory_chunk **)calloc(1024, sizeof(memory_chunk *));

    for (int i = 0; i < chunk_count; i++)
    {
        memory_chunk *curr = memory_map[i];

        if (strncmp("HOLE", curr->pid, 6) != 0)
        {
            temp[new_chunk_count] = (memory_chunk *)malloc(sizeof(memory_chunk));
            temp[new_chunk_count]->pid = (char *)calloc(6, sizeof(char));
            strncpy(temp[new_chunk_count]->pid, curr->pid, 6);

            temp[new_chunk_count]->start_address = new_start_address;
            temp[new_chunk_count]->stop_address = new_start_address + (curr->stop_address - curr->start_address);

            new_start_address = temp[new_chunk_count]->stop_address + 1;
            new_chunk_count++;
        }

        free(curr);
    }

    temp[new_chunk_count] = (memory_chunk *)malloc(sizeof(memory_chunk));
    temp[new_chunk_count]->pid = "HOLE";
    temp[new_chunk_count]->start_address = new_start_address;
    temp[new_chunk_count]->stop_address = address_space - 1;

    chunk_count = new_chunk_count + 1;
    memory_map = temp;
    free(old);
}

void stat()
{
    for (int i = 0; i < chunk_count; i++)
    {
        printf("Chunk[%d]: %s\tStart: %lu\tStop: %lu\n", i, memory_map[i]->pid, memory_map[i]->start_address, memory_map[i]->stop_address);
        fflush(stdout);
    }
}

void request_memory(char *pid, unsigned long size, char algorithm)
{
    if (chunk_count == 1024)
    {
        printf("Too many processes already allocated\n");
        return;
    }

    int found_hole = 0;
    int hole = 0;

    switch (algorithm)
    {
    case 'F':
        for (int i = 0; i < chunk_count; i++)
        {
            if (strncmp(memory_map[i]->pid, "HOLE", 5) == 0 && memory_map[i]->stop_address - memory_map[i]->start_address >= size)
            {
                insert_process(pid, size, i);
                found_hole = 1;
                break;
            }
        }

        break;
    case 'B':
        int smallest_hole = address_space + 1;
        int smallest_hole_index = -1;

        for (int i = 0; i < chunk_count; i++)
        {
            if (strncmp(memory_map[i]->pid, "HOLE", 5) == 0)
            {
                hole = memory_map[i]->stop_address - memory_map[i]->start_address;
                if (hole >= size && hole < smallest_hole)
                {
                    smallest_hole_index = i;
                    break;
                }
            }
        }

        if (smallest_hole_index != -1)
        {
            insert_process(pid, size, smallest_hole_index);
            found_hole = 1;
        }
        break;
    case 'W':
        int biggest_hole = 0;
        int biggest_hole_index = -1;

        for (int i = 0; i < chunk_count; i++)
        {
            if (strncmp(memory_map[i]->pid, "HOLE", 5) == 0)
            {
                hole = memory_map[i]->stop_address - memory_map[i]->start_address;
                if (hole >= size && hole > biggest_hole)
                {
                    biggest_hole_index = i;
                    break;
                }
            }
        }

        if (biggest_hole_index != -1)
        {
            insert_process(pid, size, biggest_hole_index);
            found_hole = 1;
        }
        break;
    default:
        break;
    }

    if (found_hole == 0)
    {
        printf("Unable to allocate memory!\n");
    }
}

void insert_process(char *pid, unsigned long size, int hole_index)
{
    if (chunk_count > 1023)
    {
        printf("Too many chunks already!\n");
        return;
    }

    printf("Inserting %s of size %lu at %d\n", pid, size, hole_index);

    memory_chunk *hole = memory_map[hole_index];

    if (hole->stop_address - size == hole->start_address)
    {
        printf("Hole = size, replacing.\n");
        strncpy(hole->pid, pid, 6);
        printf("Replaced.\n");
        return;
    }
    else
    {
        unsigned long original_hole_stop_address = hole->stop_address;

        // Change hole's new stop to be: old stop - size - 1;
        hole->stop_address = original_hole_stop_address - size - 1;

        // Assign new proc stop: hole stop, start: hole stop - size
        memory_chunk *new_proc = (memory_chunk *)malloc(sizeof(memory_chunk));

        new_proc->pid = (char *)calloc(6, sizeof(char));
        strncpy(new_proc->pid, pid, 6);

        new_proc->start_address = original_hole_stop_address - size;

        new_proc->stop_address = original_hole_stop_address;

        // Move rest of map forward
        memory_chunk **temp = (memory_chunk **)calloc(1024, sizeof(memory_chunk *));

        // Before hole
        memcpy(temp, memory_map, (hole_index + 1) * sizeof(memory_chunk *));

        // New process
        temp[hole_index + 1] = new_proc;

        // Rest of map
        if (chunk_count > hole_index + 1)
        {
            memcpy(temp + hole_index + 2, memory_map + hole_index + 1, (chunk_count - (hole_index + 1)) * sizeof(memory_chunk *));
        }

        memory_chunk **old = memory_map;
        memory_map = temp;
        free(old);
    }

    chunk_count++;
}

void release_memory(char *pid)
{
    for (int i = 0; i < chunk_count; i++)
    {
        if (strncmp(pid, memory_map[i]->pid, 6) == 0)
        {
            // Check if hole to left
            if (chunk_count > 1 && i - 1 >= 0 && strncmp("HOLE", memory_map[i - 1]->pid, 6) == 0)
            {
                remove_process(i, i - 1);
                break;
            }

            // Check if hole to right
            if (chunk_count > 1 && i + 1 < chunk_count && strncmp("HOLE", memory_map[i + 1]->pid, 6) == 0)
            {
                remove_process(i, i + 1);
                break;
            }

            // Make a new hole
            strncpy(memory_map[i]->pid, "HOLE", 6);
            break;
        }
    }
}

void remove_process(int process_index, int hole_index)
{
    printf("Removing process at %d and merging memory into hole ag %d\n", process_index, hole_index);

    int new_hole = -1;

    memory_chunk **old = memory_map;
    memory_chunk **temp = (memory_chunk **)calloc(1024, sizeof(memory_chunk *));

    memory_chunk *hole = memory_map[hole_index];
    memory_chunk *process = memory_map[process_index];

    // Adjust size of hole
    if (hole->stop_address < process->start_address)
    {
        hole->stop_address = process->stop_address;
    }
    else
    {
        hole->start_address = process->start_address;
    }

    // Remove process
    free(process);

    // Shift map down
    if (process_index == 0)
    {
        memcpy(temp, memory_map + 1, (chunk_count - 1) * sizeof(memory_chunk *));
    }
    else if (process_index == chunk_count - 1)
    {
        memcpy(temp, memory_map, chunk_count - 1 * sizeof(memory_chunk *));
    }
    else
    {
        printf("Copying 0 to %d from memory map into temp\n", process_index);
        memcpy(temp, memory_map, (process_index) * sizeof(memory_chunk *));
        printf("Copying %d to %d from memory map into temp\n", process_index + 1, chunk_count - process_index);
        memcpy(temp + process_index, memory_map + process_index + 1, (chunk_count - process_index - 1) * sizeof(memory_chunk *));
    }

    memory_map = temp;
    free(old);

    chunk_count--;
}

void parse_line(char *line, char *command, char *pid, unsigned long *size, char *algorithm)
{
    int i = 0, parsed_id = 0;
    char *entry;

    entry = strtok(line, " ");
    strncpy(command, entry, COMMAND_SIZE);
    command[COMMAND_SIZE - 1] = '\0';

    if (strncmp(command, "RQ", 2) == 0)
    {
        entry = strtok(NULL, " ");
        strncpy(pid, entry, 6);
        pid[5] = '\0';

        entry = strtok(NULL, " ");
        unsigned long tmp = strtoul(entry, NULL, 0);
        *size = tmp;

        entry = strtok(NULL, " ");
        *algorithm = entry[0];
    }
    else if (strncmp(command, "RL", 2) == 0)
    {
        entry = strtok(NULL, "\n");
        strncpy(pid, entry, 6);
        pid[5] = '\0';
    }
}
