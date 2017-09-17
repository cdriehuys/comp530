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
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


const int CHAR_POINTER_SIZE = sizeof (char*);
const char* WHITESPACE = " \t\n";


/**
 * Get a line of input from stdin.
 *
 * Input is retrieved using 'getline'.
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


/**
 * Tokenize the provided input.
 *
 * This function splits the given line into 'arguments' where each argument is
 * separated by whitespace.
 *
 * Args:
 *     line:
 *         A pointer to the string to tokenize. This string will be modified.
 *
 * Returns:
 *     An array containing pointers to each argument. The last element in the
 *     array is NULL.
 */
char** tokenize(char *line) {
    char** tokenized = NULL;

    // Perform the first split
    char* current_token = strtok(line, WHITESPACE);
    int num_spaces = 0;

    while (current_token) {
        // We have another token, so we need to expand our tokenized array
        num_spaces++;
        tokenized = realloc(tokenized, CHAR_POINTER_SIZE * num_spaces);

        tokenized[num_spaces - 1] = current_token;

        // Pull the next token, if any, from the above 'strtok' call.
        current_token = strtok(NULL, WHITESPACE);
    }

    // After tokenizing the input, we need to add a NULL as the last entry
    tokenized = realloc(tokenized, CHAR_POINTER_SIZE * (num_spaces + 1));
    tokenized[num_spaces] = NULL;

    return tokenized;
}


int main() {
    while (1) {
        char *line;

        printf("%% ");

        if (get_input(&line) == -1) {
            break;
        } else {
            pid_t child_pid = fork();

            if (child_pid == 0) {
                // Child process tokenizes the input and executes it.
                char** args = tokenize(line);
                int status = execvp(args[0], args);

                // If we've reached this point, execution failed, so we should
                // display the error.
                perror("Execution Error");

                // Since we have the opportunity, free the memory we allocated.
                free(args);

                // Exit the child with the status code given by 'execvp'.
                exit(status);
            } else {
                // The parent should just wait for the child to complete.
                wait(NULL);
            }
        }
    }

    return 0;
}
