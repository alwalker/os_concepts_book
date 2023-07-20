#include <stdio.h>
#include <stdlib.h>
#include "pid_manager.h"

bitmap *map;

int allocate_map()
{
  if (map != NULL)
  {
    fprintf(stderr, "Map alread exists");
    return 1;
  }

  map = malloc(sizeof(bitmap));
  map->num_bits = MAX_PID - MIN_PID;
  map->num_words = (map->num_bits + 32 - 1) / 32;
  map->array = calloc(map->num_words, sizeof(unsigned int));

  return 0;
}

int allocate_pid()
{
  int next_available_pid = -1;
  for (int i = 0; i <= map->num_bits; i++)
  {
    int word = i >> bitmap_shift;
    int position = i & bitmap_mask;
    if (((map->array[word] >> position) & 1) == 0)
    {
      next_available_pid = i;
      break;
    }
  }

  if (next_available_pid != -1)
  {
    int word = next_available_pid >> bitmap_shift;
    int position = next_available_pid & bitmap_mask;
    map->array[word] |= (unsigned int)1 << position;

    next_available_pid = next_available_pid + MIN_PID;
  }

  return next_available_pid;
}

void release_pid(int pid)
{
  int word = (pid - MIN_PID) >> bitmap_shift;
  int position = (pid - MIN_PID) & bitmap_mask;
  map->array[word] &= ~((unsigned int)1 << position);
}

void print_map()
{
  for (int i = 0; i < map->num_words; i++)
  {
    printf(" "
           "%08x",
           map->array[i]);
  }
  printf("\n");
}