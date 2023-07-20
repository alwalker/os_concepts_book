// Largely lifted from this:
// https://www.cse.psu.edu/~deh25/cmpsc473/assignments/HW1/bitmap.c

#define MIN_PID 300
#define MAX_PID 5000

#define bitmap_shift 5
#define bitmap_mask 31
#define bitmap_wordlength 32

typedef struct
{
    int num_bits;
    int num_words;
    unsigned int *array;
} bitmap;

int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);

void print_map();