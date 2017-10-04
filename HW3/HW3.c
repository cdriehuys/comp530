/* COMP 530 - Assignment 3
 * Chathan Driehuys
 *
 * Description:
 *   This program uses different threads to accomplish the task of reading in
 *   data, processing it, and outputing it in a specific format.
 *
 * UNC Honor Pledge: I certify that no unauthorized assistance has been
 * received or given in the completion of this work.
 */

#include <stdio.h>

#include "buffer.h"
#include "st.h"

// Default streams
#define INPUT_STREAM stdin;
#define OUTPUT_STREAM stdout;

// Other Settings
#define LINE_LENGTH 80


// Structure to hold thread info
typedef struct {
    buffer* input_buffer;
    buffer* output_buffer;
} ThreadInit;


// Declare thread start functions
void* producer_logic(void* state);
void* newline_replacer(void * state);
void* asterisk_replacer(void * state);
void* consumer_logic(void* state);


/**
 * Run the program.
 *
 * The main function initializes each processing thread and starts them.
 */
int main() {
    // Initialize ST library
    st_init();

    // Create shared buffers to pass data along the processing chain.
    buffer input_buffer;
    initializeBuffer(&input_buffer);

    buffer input_buffer2;
    initializeBuffer(&input_buffer2);

    buffer input_buffer3;
    initializeBuffer(&input_buffer3);

    // Create structures used to initialize threads
    ThreadInit init_prod = {
        NULL,
        &input_buffer
    };

    ThreadInit init_newline = {
        &input_buffer,
        &input_buffer2
    };

    ThreadInit init_asterisk = {
        &input_buffer2,
        &input_buffer3
    };

    ThreadInit init_consumer = {
        &input_buffer3,
        NULL
    };

    // Create threads
    st_thread_create(producer_logic, &init_prod, 0, 0);
    st_thread_create(newline_replacer, &init_newline, 0, 0);
    st_thread_create(asterisk_replacer, &init_asterisk, 0, 0);
    st_thread_create(consumer_logic, &init_consumer, 0, 0);

    // Wait for threads to complete
    st_thread_exit(NULL);

    return 0;
}


/**
 * Read characters from an input stream and write them to a buffer.
 *
 * The thread terminates once an EOF character is received.
 */
void* producer_logic(void* state) {
    ThreadInit* init = state;

    while (1) {
        // Read next character from input stream
        char c = fgetc(INPUT_STREAM);

        // Deposit it in the shared buffer to be processed
        deposit(init->output_buffer, c);

        // If the character was EOF, terminate the thread.
        if (c == EOF) {
            st_thread_exit(NULL);
        }
    }
}


/**
 * Move characters between buffers, replacing newlines with spaces.
 *
 * The thread terminates once an EOF character is received.
 */
void* newline_replacer(void* state) {
    ThreadInit* init = state;

    while (1) {
        char c = remoove(init->input_buffer);

        if (c == '\n') {
            c = ' ';
        }

        deposit(init->output_buffer, c);

        if (c == EOF) {
            st_thread_exit(NULL);
        }
    }
}


/**
 * Move characters between buffers, replacing double asterisks with a caret.
 *
 * The thread terminates once an EOF character is received.
 */
void* asterisk_replacer(void* state) {
    ThreadInit* init = state;

    // Keep track of the last character to determine if it was an asterisk
    char last_char = '\0';

    while (1) {
        char c = remoove(init->input_buffer);

        if (c == '*' && last_char == '*') {
            // We found a double asterisk, so deposit a caret
            deposit(init->output_buffer, '^');
            last_char = '\0';
        } else {
            if (last_char != '\0') {
                // We didn't hit a double asterisk, but the previous character
                // still needs to be deposited.
                deposit(init->output_buffer, last_char);
            }

            // Update the last character to the current one
            last_char = c;
        }

        // We still need to deposit an EOF to let other threads know that they
        // should stop.
        if (c == EOF) {
            deposit(init->output_buffer, c);

            st_thread_exit(NULL);
        }
    }
}


/**
 * Pull characters from a buffer and output them as fixed length lines.
 *
 * Lines are not output unless they are full. When an EOF is received, the
 * thread terminates.
 */
void* consumer_logic(void* state) {
    ThreadInit* init = state;

    // Create a buffer to store a complete line
    char line_buffer[LINE_LENGTH + 1];
    line_buffer[LINE_LENGTH] = '\0';

    int index = 0;

    while (1) {
        char c = remoove(init->input_buffer);

        if (c == EOF) {
            st_thread_exit(NULL);
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
