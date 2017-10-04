#include <stdio.h>
#include "buffer.h"
#include "semaphore.h"


void initializeBuffer(buffer* buf) {
    printf("Initializing buffer with %d cells\n", BUFFER_SIZE);

    buf->in = 0;
    buf->out = 0;

    createSem(&buf->emptyCells, BUFFER_SIZE);
    createSem(&buf->fullCells, 0);
}


void deposit(buffer* buf, char c) {
    down(&buf->emptyCells);

    buf->data[buf->in] = c;
    buf->in = (buf->in + 1) % BUFFER_SIZE;

    up(&buf->fullCells);
}


char remoove(buffer* buf) {
    down(&buf->fullCells);

    char c = buf->data[buf->out];
    buf-> out = (buf->out + 1) % BUFFER_SIZE;

    up(&buf->emptyCells);

    return c;
}
