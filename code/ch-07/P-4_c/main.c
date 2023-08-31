#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "buffer.h"

void *producer(void *param);
void *consumer(void *params);

sem_t empty, full;
pthread_mutex_t buffer_lock;

int main(int argc, char *argv[])
{
    int num_producers, num_consumers, seconds_till_termination;
    srand(time(NULL));
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&buffer_lock, NULL);

    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
    if (argc != 4)
    {
        fprintf(stderr, "Invalid number of arguments. Please supply:\n \
            1. Number of seconds to wait before terminating \n \
            2. Number of producer threads\n \
            3. Number of consumer threads\n");
        return -1;
    }

    seconds_till_termination = atoi(argv[1]);
    num_producers = atoi(argv[2]);
    num_consumers = atoi(argv[3]);

    /* 2. Initialize buffer */
    init_buffer();

    /* 3. Create producer thread(s) */
    pthread_t producer_threads[num_producers];
    pthread_attr_t producer_attributes[num_producers];
    for (int i = 0; i < num_producers; i++)
    {
        pthread_attr_init(&producer_attributes[i]);
        pthread_create(&producer_threads[i], &producer_attributes[i], producer, &i);
    }

    /* 4. Create consumer thread(s) */
    pthread_t consumer_threads[num_consumers];
    pthread_attr_t consumer_attributes[num_consumers];
    for (int i = 0; i < num_consumers; i++)
    {
        pthread_attr_init(&consumer_attributes[i]);
        pthread_create(&consumer_threads[i], &consumer_attributes[i], consumer, &i);
    }

    /* 5. Sleep */
    sleep(seconds_till_termination);

    /* 6. Exit */
    return 0;
}

void *producer(void *params)
{
    buffer_item item;
    int id = *(int *)params;

    printf("Producer %d: starting\n", id);

    for (;;)
    {
        sleep((rand() % 2) + 1);

        item = rand() % 50;

        printf("Producer %d: Is waiting for empty signal\n", id);
        sem_wait(&empty);
        printf("Producer %d: Is locking buffer mutex\n", id);
        pthread_mutex_lock(&buffer_lock);

        if (insert_item(item))
        {
            fprintf(stderr, "Producer %d: Error inserting %d\n", id, item);
        }
        else
        {
            printf("Producer %d: produced %d\n", id, item);
            sem_post(&full);
        }

        printf("Producer %d: Is unlocking buffer mutex\n", id);
        pthread_mutex_unlock(&buffer_lock);
    }
}

void *consumer(void *params)
{
    buffer_item item = -1;
    int id = *(int *)params;

    printf("Consumer %d: starting\n", id);

    for (;;)
    {
        sleep((rand() % 3) + 2);

        printf("Consumer %d: Is waiting for full signal\n", id);
        sem_wait(&full);
        printf("Consumer %d: Is locking buffer mutex\n", id);
        pthread_mutex_lock(&buffer_lock);

        if (remove_item(&item))
        {
            fprintf(stderr, "Consumer %d: Error removing item\n", id, item);
        }
        else
        {
            printf("Consumer %d: consumed %d\n", id, item);
            sem_post(&empty);
        }

        printf("Consumer %d: Is unlocking buffer mutex\n", id);
        pthread_mutex_unlock(&buffer_lock);
    }
}