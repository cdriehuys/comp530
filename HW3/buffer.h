#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "semaphore.h"

#define BUFFER_SIZE 5


typedef struct buffer {
    char data[BUFFER_SIZE];
    int in;
    int out;
    semaphore emptyCells;
    semaphore fullCells;
} buffer;

void initializeBuffer(buffer* buf);

void deposit(buffer* buf, char c);

char remoove(buffer* buf);

#endif
