/* COMP 530 - Assignment 4
 * Chathan Driehuys
 *
 * Description:
 *   This program uses different processes to accomplish the task of reading in
 *   data, processing it, and outputing it in a specific format. Pipes are used
 *   to pass data between processes.
 *
 * UNC Honor Pledge: I certify that no unauthorized assistance has been
 * received or given in the completion of this work.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


// Number of processes other than the ones doing text replacement. This
// includes the reading and writing processes.
#define EXTRA_PROCESSES 2

// The stream to read input from.
#define INPUT_STREAM stdin

// Length of the line to buffer before outputting it.
#define LINE_LENGTH 80


// Define signature for functions that modify the inputted text.
typedef void (*replacer_func)(int, int);


void asterisk_replacer(int input_descriptor, int output_descriptor);
void newline_replacer(int input_descriptor, int output_descriptor);
void reader_logic(int output_descriptor);
void writer_logic(int input_descriptor);


// An array containing the functions to run against the inputted text. More
// text modifications can be done by creating the necessary function and adding
// it to the array.
replacer_func replacer_funcs[] = {
    asterisk_replacer,
    newline_replacer
};


/**
 * Run the main process to spawn and track each child process.
 *
 * The process completes when each child process returns.
 */
int main() {
    int num_replacers = sizeof(replacer_funcs) / sizeof(replacer_funcs[0]);
    // We use the number of 'replacer' functions to determine how many
    // processes are necessary.
    int num_processes = num_replacers + EXTRA_PROCESSES;
    // We need one less pipe than process since a pipe is used to communicate
    // between two processes.
    int num_pipes = num_processes - 1;

    int i;
    int pipes[num_pipes][2];

    // Initialize the necessary number of pipes
    for (i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pids[num_processes];

    // Create the appropriate processes
    for (i = 0; i < num_processes; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            if (i == 0) {
                // The first process is responsible for reading in characters
                // from the input stream.
                reader_logic(pipes[0][1]);
            } else if (i == num_processes - 1) {
                // The last process is responsible for outputting the
                // transformed characters it receives.
                writer_logic(pipes[i - 1][0]);
            } else {
                // Each 'replacer' function gets its own process.
                replacer_funcs[i - 1](pipes[i - 1][0], pipes[i][1]);
            }

            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all child processes to complete
    while (i > 0) {
        wait(NULL);
        i--;
    }

    return 0;
}


/**
 * Move characters bettween pipes, replacing double asterisks with a caret.
 *
 * Args:
 *     input_descriptor:
 *         An integer representing which pipe to read from.
 *     output_descriptor:
 *         An integer representing which pipe to output to.
 */
void asterisk_replacer(int input_descriptor, int output_descriptor) {
    char c;
    char last_char = '\0';

    while (1) {
        read(input_descriptor, &c, sizeof(char));

        if (c == '*' && last_char == '*') {
            // We found a double asterisk, so replace it with a caret
            c = '^';
            write(output_descriptor, &c, sizeof(char));
            last_char = '\0';
        } else {
            if (last_char != '\0') {
                // We didn't hit a double asterisk, so write the previously
                // stored character.
                write(output_descriptor, &last_char, sizeof(char));
            }

            last_char = c;
        }

        // If we receieved an EOF, write it so other processes can receive it
        // and then terminate the function.
        if (c == EOF) {
            write(output_descriptor, &c, sizeof(char));

            return;
        }
    }
}


/**
 * Move characters between pipes, replacing newlines with spaces.
 *
 * Args:
 *     input_descriptor:
 *         An integer representing which pipe to read from.
 *     output_descriptor:
 *         An integer representing which pipe to output to.
 */
void newline_replacer(int input_descriptor, int output_descriptor) {
    char c;

    do {
        // Pull next character from pipe
        read(input_descriptor, &c, sizeof(char));

        // Do replacement if necessary
        if (c == '\n') {
            c = ' ';
        }

        // Write the (replaced) character to the output pipe
        write(output_descriptor, &c, sizeof(char));
    } while (c != EOF);
}


/**
 * Read characters in and pass them to a pipe.
 *
 * Args:
 *     output_descriptor:
 *         An integer representing the pipe to output to.
 */
void reader_logic(int output_descriptor) {
    char c;

    do {
        c = fgetc(INPUT_STREAM);
        write(output_descriptor, &c, sizeof(char));
    } while (c != EOF);
}


/**
 * Print received characters line-by-line.
 *
 * Characters are read into a buffer. The buffer is only printed once it is
 * full.
 *
 * Args:
 *     input_descriptor:
 *         An integer representing the pipe to read input from.
 */
void writer_logic(int input_descriptor) {
    char c;
    char line[LINE_LENGTH];
    int index = 0;

    while (1) {
        // Read the next character from the pipe
        read(input_descriptor, &c, sizeof(char));

        // If it's an EOF we want to exit without storing the character
        if (c == EOF) {
            return;
        }

        line[index] = c;

        // Print and reset the buffer if it's full, otherwise move on to the
        // next cell in the buffer.
        if (index == LINE_LENGTH - 1) {
            printf("%s\n", line);
            index = 0;
        } else {
            index += 1;
        }
    }
}
