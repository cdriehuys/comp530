/* COMP 530 - Assignment 6
 * Chathan Driehuys
 *
 * This program uses memory mapped files to implement a text processing
 * pipeline to read text, perform some transformations, and output it.
 *
 * References:
 *   Basic mmap code:
 *     https://www.cs.unc.edu/~jeffay/courses/comp530/homework/HW6/mmap-example.c
 *
 *   Semaphore Usage:
 *     http://www.csc.villanova.edu/~mdamian/threads/posixsem.html
 *
 *   mmap Usage:
 *     https://www.systutorials.com/docs/linux/man/2-mmap/
 *     http://beej.us/guide/bgipc/output/html/multipage/mmap.html
 *
 *
 * UNC Honor Pledge: I certify that no unauthorized assistance has been
 * received or given in the completion of this work.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "buffer.h"
#include "semaphore.h"

// Default streams
#define INPUT_STREAM stdin
#define OUTPUT_STREAM stdout

// Line length to accumulate before printing
#define LINE_LENGTH 80

// Constant containing error code for mmap
#define ERROR -1


// Structure to hold thread info
typedef struct {
    buffer_t* input_buffer;
    buffer_t* output_buffer;
} state_t;


buffer_t* create_mmap(size_t size);
void deleteMMAP(void*);

pid_t forkChild(void (*func)(state_t*), state_t* state);
void waitForChildren(pid_t*);


// Declare thread start functions
void reader(state_t* state);
void newline_replacer(state_t* state);
void asterisk_replacer(state_t* state);
void writer(state_t* state);


/**
 * Run the program.
 *
 * The main function initializes each processing thread and starts them.
 */
int main() {
    int i;

    // Create shared buffers to pass data along the processing chain.
    buffer_t* input_buffer = create_mmap(sizeof(buffer_t));
    initializeBuffer(input_buffer);

    buffer_t* input_buffer2 = create_mmap(sizeof(buffer_t));
    initializeBuffer(input_buffer2);

    buffer_t* input_buffer3 = create_mmap(sizeof(buffer_t));
    initializeBuffer(input_buffer3);

    // Create structures used to initialize threads
    state_t state1 = {
        NULL,
        input_buffer
    };

    state_t state2 = {
        input_buffer,
        input_buffer2
    };

    state_t state3 = {
        input_buffer2,
        input_buffer3
    };

    state_t state4 = {
        input_buffer3,
        NULL
    };

    pid_t child_pids[4];

    child_pids[0] = forkChild(reader, &state1);
    child_pids[1] = forkChild(newline_replacer, &state2);
    child_pids[2] = forkChild(asterisk_replacer, &state3);
    child_pids[3] = forkChild(writer, &state4);

    waitForChildren(child_pids);

    deleteMMAP(input_buffer);
    deleteMMAP(input_buffer2);
    deleteMMAP(input_buffer3);

    return 0;
}


buffer_t* create_mmap(size_t size){
  //These are the neccessary arguments for mmap. See man mmap.
  void* addr = 0;
  int protections = PROT_READ|PROT_WRITE; //can read and write
  int flags = MAP_SHARED|MAP_ANONYMOUS; //shared b/w procs & not mapped to a file
  int fd = -1; //We could make it map to a file as well but here it is not needed.
  off_t offset = 0;

  //Create memory map
  buffer_t* state =  mmap(addr, size, protections, flags, fd, offset);

  if (( void *) ERROR == state){//on an error mmap returns void* -1.
    perror("error with mmap");
    exit(EXIT_FAILURE);
  }

  return state;
}


void deleteMMAP(void* addr){
    //This deletes the memory map at given address. see man mmap
    if (ERROR == munmap(addr, sizeof(buffer_t))){
        perror("error deleting mmap");
        exit(EXIT_FAILURE);
    }
}


pid_t forkChild(void (*function)(state_t*), state_t* state) {
     //This function takes a pointer to a function as an argument
     //and the functions argument. It then returns the forked child's pid.

    pid_t childpid;
        switch (childpid = fork()) {
            case ERROR:
                perror("fork error");
                exit(EXIT_FAILURE);
            case 0:
                (*function)(state);
            default:
                return childpid;
        }
}

void waitForChildren(pid_t* childpids){
    int status;
    while(ERROR < wait(&status)){ //Here the parent waits on any child.
        if(!WIFEXITED(status)){ //If the termination err, kill all children.
            kill(childpids[0], SIGKILL);
            kill(childpids[1], SIGKILL);
            kill(childpids[2], SIGKILL);
            kill(childpids[3], SIGKILL);
            break;
        }
    }
}


/**
 * Read characters from an input stream and write them to a buffer.
 *
 * The thread terminates once an EOF character is received.
 */
void reader(state_t* state) {
    while (1) {
        // Read next character from input stream
        char c = fgetc(INPUT_STREAM);

        // Deposit it in the shared buffer to be processed
        deposit(state->output_buffer, c);

        // If the character was EOF, terminate the thread.
        if (c == EOF) {
            exit(EXIT_SUCCESS);
        }
    }
}


/**
 * Move characters between buffers, replacing newlines with spaces.
 *
 * The thread terminates once an EOF character is received.
 */
void newline_replacer(state_t* state) {
    while (1) {
        char c = remoove(state->input_buffer);

        if (c == '\n') {
            c = ' ';
        }

        deposit(state->output_buffer, c);

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        }
    }
}


/**
 * Move characters between buffers, replacing double asterisks with a caret.
 *
 * The thread terminates once an EOF character is received.
 */
void asterisk_replacer(state_t* state) {
    // Keep track of the last character to determine if it was an asterisk
    char last_char = '\0';

    while (1) {
        char c = remoove(state->input_buffer);

        if (c == '*' && last_char == '*') {
            // We found a double asterisk, so deposit a caret
            deposit(state->output_buffer, '^');
            last_char = '\0';
        } else {
            if (last_char != '\0') {
                // We didn't hit a double asterisk, but the previous character
                // still needs to be deposited.
                deposit(state->output_buffer, last_char);
            }

            // Update the last character to the current one
            last_char = c;
        }

        // We still need to deposit an EOF to let other threads know that they
        // should stop.
        if (c == EOF) {
            deposit(state->output_buffer, c);

            exit(EXIT_SUCCESS);
        }
    }
}


/**
 * Pull characters from a buffer and output them as fixed length lines.
 *
 * Lines are not output unless they are full. When an EOF is received, the
 * thread terminates.
 */
void writer(state_t* state) {
    // Create a buffer to store a complete line
    char line_buffer[LINE_LENGTH + 1];
    line_buffer[LINE_LENGTH] = '\0';

    int index = 0;

    while (1) {
        char c = remoove(state->input_buffer);

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        }

        line_buffer[index] = c;

        // If we've completed a full line, output it, otherwise increment the
        // index and continue.
        if (index == LINE_LENGTH - 1) {
            fprintf(OUTPUT_STREAM, "%s\n", line_buffer);
            index = 0;
        } else {
            index += 1;
        }
    }
}
