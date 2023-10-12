#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "Wrong number of arguments, please only pass in memory address\n");
        exit(1);
    }

    unsigned long address = strtoul(argv[1], NULL, 0);

    unsigned long page = address/4096;

    unsigned long offset = address - (4096 * page);

    printf("\nThe address %lu contains:\npage number: %lu\noffset: %lu\n", address, page, offset);
}