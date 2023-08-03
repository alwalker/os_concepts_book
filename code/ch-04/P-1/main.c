#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int starting_row_index;
    int starting_column_index;
    int *result;
} parameters;

void read_sudoku_from_file(char *file_name);
void *check_row(void *param);
void *check_column(void *param);
void *check_square(void *param);

int sudoku[9][9];

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: a.out <path to sudoku file>\n");
        return -1;
    }

    int validation_results[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    read_sudoku_from_file(argv[1]);

    printf("THE PUZZLE\n\n");
    fflush(stdout);
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            printf("%d", sudoku[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    /*CHECK ROWS*/
    printf("\n\nChecking Rows\n");
    parameters *row_checker_params = (parameters *)malloc(sizeof(parameters));
    row_checker_params->starting_row_index = 0;
    row_checker_params->starting_column_index = 0;
    row_checker_params->result = &validation_results[0];

    pthread_t row_checker_tid;
    pthread_attr_t row_checker_attr;
    pthread_attr_init(&row_checker_attr);
    pthread_create(&row_checker_tid, &row_checker_attr, check_row, row_checker_params);

    /*CHECK COLS*/
    printf("\n\nChecking Columns\n");
    parameters *col_checker_params = (parameters *)malloc(sizeof(parameters));
    col_checker_params->starting_row_index = 0;
    col_checker_params->starting_column_index = 0;
    col_checker_params->result = &validation_results[1];

    pthread_t col_checker_tid;
    pthread_attr_t col_checker_attr;
    pthread_attr_init(&col_checker_attr);
    pthread_create(&col_checker_tid, &col_checker_attr, check_column, col_checker_params);

    pthread_join(row_checker_tid, NULL);
    pthread_join(col_checker_tid, NULL);

    /*CHECK Squares*/
    pthread_t square_threads[9];
    parameters *square_params[9];
    pthread_attr_t square_attributes[9];
    printf("\n\nChecking Squares\n");
    for (int i = 0; i < 9; i++)
    {
        int row = 0;
        if (i < 3)
        {
            row = 0;
        }
        else if (i < 6)
        {
            row = 3;
        }
        else
        {
            row = 6;
        }

        square_params[i] = (parameters *)malloc(sizeof(parameters));
        square_params[i]->starting_row_index = row;
        square_params[i]->starting_column_index = (i * 3) % 9;
        square_params[i]->result = &validation_results[i + 2];

        pthread_attr_init(&square_attributes[i]);
        pthread_create(&square_threads[i], &square_attributes[i], check_square, square_params[i]);
    }
    for (int i = 0; i < 9; i++)
    {
        pthread_join(square_threads[i], NULL);
    }

    printf("\n\nTHE RESULTS\n");
    for (int i = 0; i < 11; i++)
    {
        switch (i)
        {
        case 0:
            printf("Got %d for checking row\n", validation_results[i]);
            break;
        case 1:
            printf("Got %d for checking col\n", validation_results[i]);
            break;
        default:
            printf("Got %d for square %d\n", validation_results[i], i - 2);
        }
    }
}

void read_sudoku_from_file(char *file_name)
{
    FILE *puzzle_file = fopen(file_name, "r");
    if (puzzle_file == NULL)
    {
        fprintf(stderr, "Error reading puzzle file\n");
    }

    char *line = NULL;
    size_t len = 0;
    int row = 0;
    int col = 0;
    char *entry;

    while ((getline(&line, &len, puzzle_file)) != -1)
    {
        entry = strtok(line, " ");

        while (entry != NULL)
        {
            sudoku[row][col] = atoi(entry);

            entry = strtok(NULL, " ");
            col++;
        }

        row++;
        col = 0;
    }

    fclose(puzzle_file);
    if (line)
    {
        free(line);
    }
}

void *check_row(void *param)
{
    parameters *parsed_params = (parameters *)param;
    int all_rows_valid[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = parsed_params->starting_row_index; i < 9; i++)
    {
        int results[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        int valid = 1;

        for (int j = parsed_params->starting_column_index; j < 9; j++)
        {
            printf("Row at position %d has value %d\n", j, (sudoku[i][j]));
            results[(sudoku[i][j] - 1)] = 1;
        }

        for (int i = 0; i < 9; i++)
        {
            printf("Found %d: %d\n", i + 1, results[i]);

            if (results[i] != 1)
            {
                valid = 0;
            }
        }

        if (valid != 1)
        {
            printf("Row %d is invalid\n", i);
        }
        else
        {
            printf("Row %d is valid\n", i);
            all_rows_valid[i] = 1;
        }

        printf("\n");
    }

    for (int i = 0; i < 9; i++)
    {
        if (all_rows_valid[i] != 1)
        {
            return NULL;
        }
    }

    *parsed_params->result = 1;
}

void *check_column(void *param)
{
    parameters *parsed_params = (parameters *)param;
    int all_rows_valid[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = parsed_params->starting_column_index; i < 9; i++)
    {
        int results[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        int valid = 1;

        for (int j = parsed_params->starting_row_index; j < 9; j++)
        {
            printf("Col at position %d has value %d\n", j, (sudoku[j][i]));
            results[(sudoku[j][i] - 1)] = 1;
        }

        for (int i = 0; i < 9; i++)
        {
            printf("Found %d: %d\n", i + 1, results[i]);

            if (results[i] != 1)
            {
                valid = 0;
            }
        }

        if (valid != 1)
        {
            printf("Column %d is invalid\n", i);
        }
        else
        {
            printf("Column %d is valid\n", i);
            all_rows_valid[i] = 1;
        }

        printf("\n");
    }

    for (int i = 0; i < 9; i++)
    {
        if (all_rows_valid[i] != 1)
        {
            return NULL;
        }
    }

    *parsed_params->result = 1;
}

void *check_square(void *param)
{
    parameters *parsed_params = (parameters *)param;

    printf("Got row: %d and col: %d\n", parsed_params->starting_row_index, parsed_params->starting_column_index);

    int row, col;
    int valid = 1;
    int all_rows_valid[] = {0, 0, 0};
    int results[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < 3; i++)
    {
        int valid = 1;

        for (int j = 0; j < 3; j++)
        {
            row = parsed_params->starting_row_index + i;
            col = parsed_params->starting_column_index + j;
            printf("Entry at position %d,%d has value %d\n", row, col, (sudoku[row][col]));
            results[(sudoku[row][col] - 1)] = 1;
        }
    }

    for (int i = 0; i < 9; i++)
    {
        printf("Found %d: %d\n", i + 1, results[i]);

        if (results[i] != 1)
        {
            valid = 0;
        }
    }

    if (valid != 1)
    {
        printf("Square %d,%d is invalid\n", parsed_params->starting_row_index, parsed_params->starting_column_index);
    }
    else
    {
        printf("Square %d,%d is valid\n", parsed_params->starting_row_index, parsed_params->starting_column_index);
    }

    *parsed_params->result = valid;
}
