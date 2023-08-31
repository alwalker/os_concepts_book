#include "buffer.h"

buffer_item buffer[BUFFER_SIZE];

void init_buffer()
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = EMPTY_SPACE;
    }
}

int insert_item(buffer_item item)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (buffer[i] == EMPTY_SPACE)
        {
            buffer[i] = item;
            return 0;
        }
    }

    return -1;
}

int remove_item(buffer_item *item)
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (buffer[i] != EMPTY_SPACE)
        {
            *item = buffer[i];
            buffer[i] = EMPTY_SPACE;

            return 0;
        }
    }

    return -1;
}