#define BUFFER_SIZE 5
#define EMPTY_SPACE -1

typedef int buffer_item;

void init_buffer();

/* insert item into buffer
return 0 if successful, otherwise
return -1 indicating an error condition */
int insert_item(buffer_item);

/* remove an object from buffer
placing it in item
return 0 if successful, otherwise
return -1 indicating an error condition */
int remove_item(buffer_item*);
