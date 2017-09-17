/* COMP 530 - Assignment 2
 * Chathan Driehuys
 *
 * UNC Honor Pledge: I certify that no unauthorized assistance has been
 * received or given in the completion of this work.
 *
 *
 * This program is a very simple Linux shell. It accepts a path to an
 * executable file along with its arguments and runs that file in a separate
 * process.
 */

#include <stdio.h>


/**
 * Get a line of input from stdin.
 *
 * Input is retrieved using getline.
 * http://man7.org/linux/man-pages/man3/getline.3.html
 *
 * Function adapted from the following Stack Overflow question:
 * https://stackoverflow.com/questions/12252103/how-to-read-a-line-from-stdin-blocking-until-the-newline-is-found
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
    // If we set the line to NULL, 'getline' will allocate enought memory to
    // hold the line automatically.
    *line = NULL;

    // We need a variable to store the size of the input in. getline will set
    // the variable to the number of bytes read.
    size_t line_size;

    return getline(line, &line_size, stdin);
}


int main() {
    char *line;

    if (get_input(&line) == -1) {
        printf("No line received\n");
    } else {
        printf("Received line: %s\n", line);
    }

    return 0;
}
