/* COMP 530 - Assignment 5
 * Chathan Driehuys
 *
 * UNC Honor Pledge: I certify that no unauthorized assistance has been
 * received or given in the completion of this work.
 *
 *
 * The client program accepts input from standard input, connects to the server
 * over a socket, sends the command to the server, and displays the output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Socket.h"


#define INPUT_STREAM stdin


int get_input(char** line);


int main(int argc, char* argv[]) {
    char* line;
    int i;
    int line_length;
    Socket connect_socket;

    if (argc < 3) {
        printf("Please provide the hostname and port of the server.\n");
        exit(EXIT_FAILURE);
    }

    connect_socket = Socket_new(argv[1], atoi(argv[2]));
    if (connect_socket < 0) {
        printf("Failed to connect to server socket.\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        /* Get a line of input from standard in. If there is no input, we exit
         * the loop.
         */
        if (get_input(&line) == -1) {
            break;
        }

        // Get line length including the null terminator
        line_length = strlen(line) + 1;

        for (i = 0; i < line_length; i++) {
            if (Socket_putc((int) line[i], connect_socket) == EOF) {
                printf("Failed to send data to server.\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    Socket_close(connect_socket);

    return 0;
}


/**
 * Get a line of input from stdin.
 *
 * Function adapted from the following Stack Overflow answer:
 * https://stackoverflow.com/a/12252195/3762084
 *
 * Args:
 *     line:
 *         A pointer to a pointer to a string. The next line of input will be
 *         stored at this pointer. This needs to be a double pointer otherwise
 *         'getline' doesn't store the output correctly.
 *
 * Returns:
 *     The result of 'getline' which is the number of characters read including
 *     the trailing '\n'. This will be -1 if no input was received (EOF).
 */
int get_input(char** line) {
    // Display prompt
    printf("%% ");

    // If we set the line to NULL, 'getline' will allocate enought memory to
    // hold the line automatically.
    *line = NULL;

    // We need a variable to store the size of the input in. getline will set
    // the variable to the number of bytes read.
    size_t line_size;

    return getline(line, &line_size, INPUT_STREAM);
}
