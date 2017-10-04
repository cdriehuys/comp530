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


void* producer_logic(void* state);
void* newline_replacer(void * state);
void* asterisk_replacer(void * state);
void* consumer_logic(void* state);


int main() {
    // Initialize ST library
    st_init();

    // Create shared buffers
    buffer input_buffer;
    initializeBuffer(&input_buffer);

    buffer input_buffer2;
    initializeBuffer(&input_buffer2);

    buffer input_buffer3;
    initializeBuffer(&input_buffer3);

    // Create structure used to initialize threads
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


void* newline_replacer(void* state) {
    ThreadInit* init = state;

    while (1) {
        // Pull the next character from the buffer
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


void* asterisk_replacer(void* state) {
    ThreadInit* init = state;

    char last_char = '\0';

    while (1) {
        // Pull the next character from the buffer
        char c = remoove(init->input_buffer);

        if (c == '*' && last_char == '*') {
            deposit(init->output_buffer, '^');
            last_char = '\0';
        } else {
            if (last_char != '\0') {
                deposit(init->output_buffer, last_char);
            }

            last_char = c;
        }

        if (c == EOF) {
            deposit(init->output_buffer, c);

            st_thread_exit(NULL);
        }
    }
}


void* consumer_logic(void* state) {
    ThreadInit* init = state;

    // Create a buffer to store a complete line
    char line_buffer[LINE_LENGTH + 1];
    line_buffer[LINE_LENGTH] = '\0';

    int index = 0;

    while (1) {
        // Pull the next character from the buffer
        char c = remoove(init->input_buffer);

        // Exit if we've reached EOF
        if (c == EOF) {
            st_thread_exit(NULL);
        }

        line_buffer[index] = c;

        // If we've completed a full line, output it
        if (index == LINE_LENGTH - 1) {
            fprintf(OUTPUT_STREAM, "%s\n", line_buffer);
            index = 0;
        } else {
            index += 1;
        }
    }
}
