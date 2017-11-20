/**
 * A buffer intended to pass data between threads.
 *
 * Data in the buffer behaves like it is in a first-in-first-out queue.
 */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "semaphore.h"

#define BUFFER_SIZE 128


typedef struct buffer {
    char data[BUFFER_SIZE];
    int in;
    int out;
    sem_t emptyCells;
    sem_t fullCells;
} buffer_t;

/**
 * Initializes an empty buffer. This must be called before using the buffer.
 *
 * Args:
 *     buf:
 *         A pointer to the buffer to initialize.
 */
void initializeBuffer(buffer_t* buf);

/**
 * Place a new character in the buffer.
 *
 * If there is no space in the buffer, the thread will wait until there is
 * space available before continuing.
 *
 * Args:
 *     buf:
 *         A pointer to the buffer to place the character in.
 *     c:
 *         The character to insert into the buffer.
 */
void deposit(buffer_t* buf, char c);

/**
 * Remove the next character from the buffer.
 *
 * If there are no characters in the buffer, the thread will wait until the
 * next character is placed in the buffer before continuing.
 *
 * Args:
 *     buf:
 *         A pointer to the buffer to remove a character from.
 *
 * Returns:
 *     The next character in the buffer.
 */
char remoove(buffer_t* buf);

#endif
