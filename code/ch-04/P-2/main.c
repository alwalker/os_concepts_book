#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *sort_sub_array(void *param);
void *merge_arrays(void *param);
void *print_array(int *, int);

typedef struct
{
    int *target;
} sort_method_params;

typedef struct
{
    int *left_half;
    int *right_half;
} merge_method_params;

int unsorted[10] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8};
int sorted[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int main(int argc, char *argv[])
{
    printf("Sorting: ");
    print_array(unsorted, 10);
    printf("\n");

    int left_half[5];
    memcpy(left_half, unsorted, 5 * sizeof(int));
    int right_half[5];
    memcpy(right_half, unsorted + 5, 5 * sizeof(int));

    /* SORT LEFT HALF */
    sort_method_params *left_sort_params = (sort_method_params *)malloc(sizeof(sort_method_params));
    left_sort_params->target = left_half;

    pthread_t left_sort_tid;
    pthread_attr_t left_sort_attr;
    pthread_attr_init(&left_sort_attr);
    pthread_create(&left_sort_tid, &left_sort_attr, sort_sub_array, left_sort_params);

    /* SORT RIGHT HALF */
    sort_method_params *right_sort_params = (sort_method_params *)malloc(sizeof(sort_method_params));
    right_sort_params->target = right_half;

    pthread_t right_sort_tid;
    pthread_attr_t right_sort_attr;
    pthread_attr_init(&right_sort_attr);
    pthread_create(&right_sort_tid, &right_sort_attr, sort_sub_array, right_sort_params);

    pthread_join(left_sort_tid, NULL);
    pthread_join(right_sort_tid, NULL);

    printf("Sorted Left Half: ");
    print_array(left_half, 5);
    printf("\n");
    printf("Sorted Right Half: ");
    print_array(right_half, 5);
    printf("\n");

    /* MERGE HALVES */
    merge_method_params *merge_params = (merge_method_params *)malloc(sizeof(merge_method_params));
    merge_params->left_half = left_half;
    merge_params->right_half = right_half;

    pthread_t merge_tid;
    pthread_attr_t merge_attr;
    pthread_attr_init(&merge_attr);
    pthread_create(&merge_tid, &merge_attr, merge_arrays, merge_params);

    pthread_join(merge_tid, NULL);

    printf("Results: ");
    print_array(sorted, 10);
    printf("\n");
}

void *print_array(int *input, int len)
{
    char *output = (char *)calloc(1024, sizeof(char));
    char *array_char = (char *)calloc(3, sizeof(char));

    for (int i = 0; i < len; i++)
    {
        sprintf(array_char, "%d ", input[i]);
        strcat(output, array_char);
    }

    printf("%s", output);
}

int compare(const void *a, const void *b)
{
    int int_a = *((int *)a);
    int int_b = *((int *)b);

    if (int_a == int_b)
        return 0;
    else if (int_a < int_b)
        return -1;
    else
        return 1;
}

void *sort_sub_array(void *param)
{
    sort_method_params *parsed_params = (sort_method_params *)param;

    // printf("Got: ");
    // print_array(parsed_params->target, 5);
    // printf("\n");

    qsort(parsed_params->target, 5, sizeof(int), compare);

    // printf("Sorted: ");
    // print_array(parsed_params->target, 5);
    // printf("\n");
}

void *merge_arrays(void *param)
{
    merge_method_params *parsed_params = (merge_method_params *)param;

    int left_index = 0;
    int right_index = 0;

    for (int i = 0; i < 10; i++)
    {
        if(left_index >= 5 && right_index < 5) {
            sorted[i] = parsed_params->right_half[right_index];
            right_index++;
            // printf("out of left\n");
        }
        else if (right_index >= 5 && left_index <5) {
            sorted[i] = parsed_params->left_half[left_index];
            left_index++;
            // printf("out of right\n");
        }
        else if (parsed_params->left_half[left_index] <= parsed_params->right_half[right_index])
        {
            sorted[i] = parsed_params->left_half[left_index];
            left_index++;
            // printf("left is less\n");
        }
        else if (parsed_params->left_half[left_index] >= parsed_params->right_half[right_index])
        {
            sorted[i] = parsed_params->right_half[right_index];
            right_index++;
            // printf("right is less\n");
        }
        else {
            // printf("shouldn't be here\n");
        }
    }
}
