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


#define EXTRA_PROCESSES 2
#define LINE_LENGTH 80


typedef void (*replacer_func)(int, int);


void asterisk_replacer(int input_descriptor, int output_descriptor);
void newline_replacer(int input_descriptor, int output_descriptor);
void reader_logic(int output_descriptor);
void writer_logic(int input_descriptor);


replacer_func replacer_funcs[] = {
    asterisk_replacer,
    newline_replacer
};


int main() {
    int num_replacers = sizeof(replacer_funcs) / sizeof(replacer_funcs[0]);
    int num_processes = num_replacers + EXTRA_PROCESSES;
    int num_pipes = num_processes - 1;

    int i;
    int pipes[num_pipes][2];

    for (i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pids[num_processes];

    for (i = 0; i < num_processes; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pids[i] == 0) {
            if (i == 0) {
                reader_logic(pipes[0][1]);
            } else if (i == num_processes - 1) {
                writer_logic(pipes[i - 1][0]);
            } else {
                replacer_funcs[i - 1](pipes[i - 1][0], pipes[i][1]);
            }
        }
    }

    int status;
    pid_t pid;

    while (i > 0) {
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        i--;
    }

    return 0;
}


void asterisk_replacer(int input_descriptor, int output_descriptor) {
    char c;

    do {
        read(input_descriptor, &c, sizeof(char));
        write(output_descriptor, &c, sizeof(char));
    } while (c != EOF);

    exit(EXIT_SUCCESS);
}


void newline_replacer(int input_descriptor, int output_descriptor) {
    char c;

    do {
        read(input_descriptor, &c, sizeof(char));
        write(output_descriptor, &c, sizeof(char));
    } while (c != EOF);

    exit(EXIT_SUCCESS);
}


void reader_logic(int output_descriptor) {
    char c;

    do {
        c = fgetc(stdin);
        write(output_descriptor, &c, sizeof(char));
    } while (c != EOF);

    exit(EXIT_SUCCESS);
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
            exit(EXIT_SUCCESS);
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
