#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

void *ta(void *param);
void *student(void *params);

sem_t *get_help_from_ta, *getting_help_from_ta;
pthread_mutex_t check_ta_busy_lock;
int ta_busy = 0;

sem_t *wait_in_chair_one, *wait_in_chair_two, *wait_in_chair_three;
pthread_mutex_t check_chair_status_lock;
int chair_status[] = {0, 0, 0};

int main(int argc, char *argv[])
{
    srand(time(NULL));

    get_help_from_ta = sem_open("GET_HELP_FROM_TA", O_CREAT, 0666, 1);
    getting_help_from_ta = sem_open("GETTING_HELP_FROM_TA", O_CREAT, 0666, 1);
    pthread_mutex_init(&check_ta_busy_lock, NULL);

    wait_in_chair_one = sem_open("WAIT_IN_CHAIR_ONE", O_CREAT, 0666, 1);
    wait_in_chair_two = sem_open("WAIT_IN_CHAIR_TWO", O_CREAT, 0666, 1);
    wait_in_chair_three = sem_open("WAIT_IN_CHAIR_THREE", O_CREAT, 0666, 1);
    pthread_mutex_init(&check_chair_status_lock, NULL);

    pthread_t ta_tid;
    pthread_attr_t ta_attr;
    pthread_attr_init(&ta_attr);
    pthread_create(&ta_tid, &ta_attr, ta, NULL);

    pthread_t student_threads[10];
    pthread_attr_t student_attributes[10];
    for (int i = 0; i < 2; i++)
    {
        pthread_attr_init(&student_attributes[i]);
        pthread_create(&student_threads[i], &student_attributes[i], student, &i);
    }

    pthread_join(ta_tid, NULL);
}

void *ta(void *params)
{
    /****************************
    if student needs help
        help
        check for more students when done
    else
        sleep
    ****************************/
    int current_chair = 0;

    for (;;)
    {
        current_chair = 0;

        // Sleeping
        printf("TA: Going to sleep\n");
        sem_wait(get_help_from_ta);

        // Wake up and help whoever is in the doorway
        pthread_mutex_lock(&check_ta_busy_lock);
        ta_busy = 1;
        pthread_mutex_unlock(&check_ta_busy_lock);

        printf("TA: Helping student\n");
        sleep((rand() % 4) + 1);

        sem_post(getting_help_from_ta);

        // Check the chairs and help whoever is there in order
        for (;;)
        {
            printf("TA: Checking chairs...\n");
            pthread_mutex_lock(&check_chair_status_lock);

            // Help whoever is in the current chair
            if (chair_status[current_chair] != 0)
            {
                printf("TA: Found student %d in chair %d\n", chair_status[current_chair], current_chair);
                // Move student from chair to office
                chair_status[current_chair] = 0;
                switch (current_chair)
                {
                case 0:
                    sem_post(wait_in_chair_one);
                    break;
                case 1:
                    sem_post(wait_in_chair_two);
                    break;
                case 2:
                    sem_post(wait_in_chair_three);
                    break;
                default:
                    printf("TA checking for 4th dimensional chair!\n");
                    break;
                }
                pthread_mutex_unlock(&check_chair_status_lock);

                // Help student
                sleep((rand() % 4) + 1);
                sem_post(getting_help_from_ta);

                current_chair = (current_chair + 1) % 3;

            } // Check if all chairs empty, if so then go back to sleep
            else if (chair_status[0] == 0 && chair_status[1] == 0 && chair_status[2] == 0)
            {
                printf("TA: All chairs empty\n");
                pthread_mutex_unlock(&check_chair_status_lock);
                break;
            } // Move to next chair
            else
            {
                printf("TA: Checking next chair\n");
                pthread_mutex_unlock(&check_chair_status_lock);
                current_chair = (current_chair + 1) % 3;
            }
        }

        pthread_mutex_lock(&check_ta_busy_lock);
        ta_busy = 0;
        pthread_mutex_unlock(&check_ta_busy_lock);
    }
}

void *student(void *params)
{
    int id = *(int *)params;
    /*******************************
     program
     if need help
        check chairs
        if chairs partially full
            take seat
        else if chairs full
            back to programming
        else
            check ta
            if not helping
                get help
            else
                take seat
    *******************************/
    for (;;)
    {
        // Program
        printf("STUDENT %d: Programming...\n", id);
        sleep((rand() % 4) + 1);
        printf("STUDENT %d: Looking for help...\n", id);

        // Look for help
        pthread_mutex_lock(&check_ta_busy_lock);
        pthread_mutex_lock(&check_chair_status_lock);

        // If TA not busy, get help
        if (ta_busy == 0)
        {
            printf("STUDENT %d: TA isn't busy, getting help\n", id);
            pthread_mutex_unlock(&check_ta_busy_lock);
            pthread_mutex_unlock(&check_chair_status_lock);

            sem_post(get_help_from_ta);
            sem_wait(getting_help_from_ta);
        } // Else, check chairs, if one is empty set in it
        else if (chair_status[0] == 0 || chair_status[1] == 0 || chair_status[2] == 0)
        {
            printf("STUDENT %d: At least one chair is free...\n", id);
            pthread_mutex_unlock(&check_ta_busy_lock);

            for (int i = 0; i < 3; i++)
            {
                if (chair_status[i] == 0)
                {
                    printf("STUDENT %d: Setting in chair %d and waiting...\n", id, i);
                    chair_status[i] = id;
                    pthread_mutex_unlock(&check_chair_status_lock);

                    // Wait in this chair for help
                    switch (i)
                    {
                    case 0:
                        sem_wait(wait_in_chair_one);
                        break;
                    case 1:
                        sem_wait(wait_in_chair_two);
                        break;
                    case 2:
                        sem_wait(wait_in_chair_three);
                        break;
                    default:
                        break;
                    }

                    // Get help
                    printf("STUDENT %d: Getting help...\n", id);
                    sem_wait(getting_help_from_ta);
                    printf("STUDENT %d: Got help\n", id);

                    break;
                }
            }
        } // Else despair and return to programming
        else
        {
            printf("STUDENT %d: TA is busy and chairs are full\n", id);
            pthread_mutex_unlock(&check_ta_busy_lock);
            pthread_mutex_unlock(&check_chair_status_lock);
        }
    }
}