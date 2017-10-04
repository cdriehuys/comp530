/**
 * Implementation of a buffer.
 *
 * See 'buffer.h' for details.
 */

#include <stdio.h>
#include "buffer.h"
#include "semaphore.h"


void initializeBuffer(buffer* buf) {
    buf->in = 0;
    buf->out = 0;

    // Initialize semaphores for boundary conditions
    createSem(&buf->emptyCells, BUFFER_SIZE);
    createSem(&buf->fullCells, 0);
}


void deposit(buffer* buf, char c) {
    // Ensure there is space in the buffer
    down(&buf->emptyCells);

    buf->data[buf->in] = c;
    buf->in = (buf->in + 1) % BUFFER_SIZE;

    // Allow the deposited data to be consumed
    up(&buf->fullCells);
}


char remoove(buffer* buf) {
    // Ensure there is an item in the buffer
    down(&buf->fullCells);

    char c = buf->data[buf->out];
    buf-> out = (buf->out + 1) % BUFFER_SIZE;

    // Allow data to be placed in the cell that was just read
    up(&buf->emptyCells);

    return c;
}
