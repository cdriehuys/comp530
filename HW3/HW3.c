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


// Structure to hold thread info
typedef struct {
    FILE* output_stream;
    buffer* b;
} ThreadInit;


void* producer_logic(void* state);
void* consumer_logic(void* state);


int main() {
    // Initialize ST library
    st_init();

    // Set output stream
    FILE* output_stream = stdout;

    // Create shared buffer
    buffer b;
    initializeBuffer(&b);

    // Create structure used to initialize threads
    ThreadInit init = {
        output_stream,
        &b
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

    int i;
    for (i = 0; i < 10; i++) {
        fprintf(init->output_stream, "Producer produced: %c\n", 'a' + i);
        fflush(init->output_stream);

        deposit(init->b, 'a' + i);

        st_usleep(333333);
    }
}


void* consumer_logic(void* state) {
    ThreadInit* init = state;

    int i;
    for (i = 0; i < 10; i++) {
        char c = remoove(init->b);

        fprintf(init->output_stream, "Consumer received: %c\n", c);
        fflush(init->output_stream);

        st_usleep(666666);
    }
}
