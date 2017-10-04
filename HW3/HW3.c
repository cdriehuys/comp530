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
#define DEFAULT_INPUT_STREAM stdin;


// Structure to hold thread info
typedef struct {
    FILE* input_stream;
    FILE* output_stream;
    buffer* buf;
} ThreadInit;


void* producer_logic(void* state);
void* consumer_logic(void* state);


int main() {
    // Initialize ST library
    st_init();

    // Set streams
    FILE* input_stream = DEFAULT_INPUT_STREAM;
    FILE* output_stream = stdout;

    // Create shared buffer
    buffer buf;
    initializeBuffer(&buf);

    // Create structure used to initialize threads
    ThreadInit init = {
        input_stream,
        output_stream,
        &buf
    };

    // Create threads
    st_thread_create(producer_logic, &init, 0, 0);
    st_thread_create(consumer_logic, &init, 0, 0);

    // Wait for threads to complete
    st_thread_exit(NULL);

    return 0;
}


void* producer_logic(void* state) {
    ThreadInit* init = state;

    while (1) {
        // Read next character from input stream
        char c = fgetc(init->input_stream);

        // Deposit it in the shared buffer to be processed
        deposit(init->buf, c);

        // If the character was EOF, terminate the thread.
        if (c == EOF) {
            st_thread_exit(NULL);
        }
    }
}


void* consumer_logic(void* state) {
    ThreadInit* init = state;

    while (1) {
        // Pull the next character from the buffer
        char c = remoove(init->buf);

        // Exit if we've reached EOF
        if (c == EOF) {
            st_thread_exit(NULL);
        }

        fprintf(init->output_stream, "Consumer received: %c\n", c);
        fflush(init->output_stream);
    }
}
