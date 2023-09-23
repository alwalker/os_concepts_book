#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *get_available_resources(int, char **);
void get_customer_spec(char *, int, int, int **, int **);
void initialize_allocation(int **, int, int);
int try_perform_request(char *, int, int, int *, int **, int **, int **);
void release(char *, int, int *, int **, int **);
void print_resources(int, int, int *, int **, int **, int **);
int check_if_safe(int, int, int *, int **, int **, int **);
int vector_compare(int *, int *, int);
void vector_add(int *, int *, int);
void vector_subtract(int *, int *, int);
void vector_parse(int *, int *, char *);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid number of arguments! Please supply an amount for each resource.\n");
        return 1;
    }

    int num_resources = argc - 1;
    int num_customers = 5;

    printf("Setting up available resources...\n");
    int *available = get_available_resources(argc, argv);
    int **maximum = (int **)calloc(num_customers, sizeof(int *));
    int **allocation = (int **)calloc(num_customers, sizeof(int *));
    int **need = (int **)calloc(num_customers, sizeof(int *));

    printf("Reading customer spec...\n");
    get_customer_spec("customer_spec.txt", num_customers, num_resources, maximum, need);
    printf("Initializing the need and allocation matrices...\n\n");
    initialize_allocation(allocation, num_customers, num_resources);

    print_resources(num_customers, num_resources, available, maximum, allocation, need);

    printf("\nAre we safe...");
    if (check_if_safe(num_customers, num_resources, available, maximum, allocation, need) == 1)
    {
        printf("Yes!\n");
    }
    else
    {
        printf("No!\n");
        exit(1);
    }
    printf("\n");

    size_t line_size;
    char *line;
    for (;;)
    {
        printf("Command:\n");
        getline(&line, &line_size, stdin);
        printf("\n");

        if (strstr(line, "RQ") != NULL)
        {
            try_perform_request(line, num_customers, num_resources, available, maximum, allocation, need);
        }
        if (strstr(line, "RL") != NULL)
        {
            release(line, num_resources, available, allocation, need);
        }
        if (strstr(line, "PR") != NULL)
        {
            print_resources(num_customers, num_resources, available, maximum, allocation, need);
        }
        if (strstr(line, "EX") != NULL)
        {
            printf("Got exit\n");
            exit(0);
        }
    }
}

void release(char *line, int num_resources, int *available, int **allocation, int **need)
{
    int *request = (int *)calloc(num_resources, sizeof(int));
    int customer_num;
    vector_parse(request, &customer_num, line + 3);

    vector_add(available, request, num_resources);
    vector_add(need[customer_num], request, num_resources);
    vector_subtract(allocation[customer_num], request, num_resources);

    free(request);
}

int try_perform_request(char *line, int num_customers, int num_resources, int *available, int **maximum, int **allocation, int **need)
{
    int *request = (int *)calloc(num_resources, sizeof(int));
    int customer_num;
    vector_parse(request, &customer_num, line + 3);

    // 1. If Requesti ≤ Needi , go to step 2. Otherwise, raise an error condition, since the thread has exceeded its maximum claim.
    if (vector_compare(request, need[customer_num], num_resources) > 0)
    {
        printf("Customer trying to exceed maximum claim\n");
        return -1;
    }
    // 2. If Requesti ≤ Available, go to step 3. Otherwise, Ti must wait, since the resources are not available.
    if (vector_compare(request, available, num_resources) > 0)
    {
        printf("Customer requesting too many resources\n");
        return -1;
    }
    // 3. Have the system pretend to have allocated the requested resources to thread Ti by modifying the state as follows:
    // Available = Available–Requesti
    vector_subtract(available, request, num_resources);
    // Allocationi = Allocationi + Requesti
    vector_add(allocation[customer_num], request, num_resources);
    // Needi = Needi –Requesti
    vector_subtract(need[customer_num], request, num_resources);
    // If the resulting resource-allocation state is safe, the transaction is com-
    // pleted, and thread Ti is allocated its resources. However, if the new state
    // is unsafe, then Ti must wait for Requesti , and the old resource-allocation
    // state is restored.
    int is_safe = check_if_safe(num_customers, num_resources, available, maximum, allocation, need);
    if (is_safe == 1)
    {
        printf("Request granted\n");
        free(request);
        return 1;
    }
    else
    {
        printf("Request denied\n");
        vector_add(available, request, num_resources);
        vector_add(need[customer_num], request, num_resources);
        vector_subtract(allocation[customer_num], request, num_resources);
        free(request);
        return -1;
    }
}

void vector_parse(int *vector, int *id, char *line)
{
    int i = 0, parsed_id = 0;
    char *entry;

    entry = strtok(line, " ");

    while (entry != NULL)
    {
        if (parsed_id == 0)
        {
            *id = atoi(entry);
            parsed_id = 1;
        }
        else
        {
            vector[i] = atoi(entry);
            i++;
        }

        entry = strtok(NULL, " ");
    }
}

int check_if_safe(int num_customers, int num_resources, int *available, int **maximum, int **allocation, int **need)
{
    // 1. Let Work and Finish be vectors of length m and n, respectively.
    // Initialize Work = Available and Finish[i] = false for i = 0, 1, ..., n − 1.
    int *work = (int *)calloc(num_resources, sizeof(int));
    memcpy(work, available, num_resources * sizeof(int));

    int *finish = (int *)calloc(num_customers, sizeof(int));

    // 2. Find an index i such that both
    // a. Finish[i] == false
    // b. Needi ≤ Work
    // If no such i exists, go to step 4.
    for (int i = 0; i < num_customers; i++)
    {
        if (finish[i] == 0 && vector_compare(need[i], work, num_resources) < 0)
        {
            // 3. Work = Work + Allocationi
            // Finish[i] = true
            finish[i] = 1;
            vector_add(work, allocation[i], num_resources);
        }
        else
        {
            printf("Found too much work\n");
            break;
        }
    } // Go to step 2.

    // 4. If Finish[i] == true for all i, then the system is in a safe state.
    int is_safe = 1;
    for (int i = 0; i < num_customers; i++)
    {
        if (finish[i] == 0)
        {
            is_safe = 0;
            break;
        }
    }

    free(work);
    free(finish);
    return is_safe;
}

int vector_compare(int *left, int *right, int length)
{
    int result = 0;

    for (int i = 0; i < length; i++)
    {
        if (left[i] <= right[i])
        {
            result = result - 1;
        }
        else if (left[i] > right[i])
        {
            result = result + 1;
        }
    }

    if (result == length)
    {
        return 1;
    }
    else if (result == (length * -1))
    {
        return -1;
    }
    else if (result == 0)
    {
        return 0;
    }
    else
    {
        return 2;
    }
}

void vector_add(int *left, int *right, int length)
{
    for (int i = 0; i < length; i++)
    {
        left[i] = left[i] + right[i];
    }
}

void vector_subtract(int *left, int *right, int length)
{
    for (int i = 0; i < length; i++)
    {
        left[i] = left[i] - right[i];
    }
}

void initialize_allocation(int **allocation, int num_customers, int num_resources)
{
    for (int i = 0; i < num_customers; i++)
    {
        allocation[i] = (int *)calloc(num_resources, sizeof(int));

        for (int j = 0; j < num_resources; j++)
        {
            allocation[i][j] = 0;
        }
    }
}

int *get_available_resources(int argc, char *argv[])
{
    int *results = (int *)calloc(argc - 1, sizeof(int));

    for (int i = 1; i < argc; i++)
    {
        results[i - 1] = atoi(argv[i]);
    }

    return results;
}

void get_customer_spec(char *file_name, int num_customers, int num_resources, int **maximum, int **need)
{
    for (int i = 0; i < num_customers; i++)
    {
        maximum[i] = (int *)calloc(num_resources, sizeof(int));
        need[i] = (int *)calloc(num_resources, sizeof(int));
    }

    FILE *customer_spec_file = fopen(file_name, "r");
    if (customer_spec_file == NULL)
    {
        fprintf(stderr, "Error customer spec\n");
    }

    char *line = NULL;
    size_t len = 0;
    int row = 0;
    int col = 0;
    char *entry;

    while ((getline(&line, &len, customer_spec_file)) != -1)
    {
        entry = strtok(line, ",");

        while (entry != NULL)
        {
            maximum[row][col] = atoi(entry);
            need[row][col] = atoi(entry);

            entry = strtok(NULL, ",");
            col++;
        }

        row++;
        col = 0;
    }

    fclose(customer_spec_file);
    if (line)
    {
        free(line);
    }
}

void print_resources(int num_customers, int num_resources, int *available, int **maximum, int **allocation, int **need)
{
    printf("Number of customers: %d\nNumber of resources: %d\n\n", num_customers, num_resources);

    printf("Available\n");
    for (int i = 0; i < num_resources; i++)
    {
        printf("%d ", available[i]);
    }

    printf("\n\nMaximum\n");
    for (int i = 0; i < num_customers; i++)
    {
        for (int j = 0; j < num_resources; j++)
        {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }

    printf("\nAllocation\n");
    for (int i = 0; i < num_customers; i++)
    {
        for (int j = 0; j < num_resources; j++)
        {
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }

    printf("\nNeed\n");
    for (int i = 0; i < num_customers; i++)
    {
        for (int j = 0; j < num_resources; j++)
        {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
}