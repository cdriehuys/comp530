/* COMP 530 - Assignment 5
 * Chathan Driehuys
 *
 * UNC Honor Pledge: I certify that no unauthorized assistance has been
 * received or given in the completion of this work.
 *
 *
 * The server program acts as a daemon for running commands from a client. The
 * server accepts commands over a socket connect, executes them, and sends the
 * output back to the client.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Socket.h"


#define STRING_CHUNK_SIZE 32
#define TEMP_FILE_NAME_SIZE 12
#define WHITESPACE " \t\n"


ServerSocket welcome_socket;
Socket connect_socket;


void exec_command_service();
char* read_socket_string();
char** tokenize(char *line);


int main(int argc, char* argv[]) {
    int child_status;
    pid_t service_pid;

    // Make sure we received the necessary arguments.
    if (argc < 2) {
        printf("Usage: HW5server <port>\n");
        exit(EXIT_FAILURE);
    }

    // Create a socket that clients can connect to in order to start a session.
    welcome_socket = ServerSocket_new(atoi(argv[1]));
    if (welcome_socket < 0) {
        printf("Failed to create welcome socket.\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Connect to a specific client
        connect_socket = ServerSocket_accept(welcome_socket);
        if (connect_socket < 0) {
            printf("Failed to accept connection to server socket.\n");
            exit(EXIT_FAILURE);
        }

        // Create a new process to handle a single client's requests
        service_pid = fork();

        if (service_pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (service_pid == 0) {
            // In the child, handle the client's requests
            exec_command_service();
            Socket_close(connect_socket);
            exit(EXIT_SUCCESS);
        } else {
            // In the parent daemon process, clean up a zombie if it exists
            Socket_close(connect_socket);
            waitpid(-1, &child_status, WNOHANG);
        }
    }

    return 0;
}


/**
 * Handle execution of a single client's requests.
 *
 * For each line received from the client, a new process is forked and the
 * client's command is executed in it. The worker process handles capturing of
 * the output and sending it back to the client.
 */
void exec_command_service() {
    char* command;
    pid_t worker_pid;

    // The child process doesn't need the welcoming socket
    Socket_close(welcome_socket);

    // Generate a tempfile name and redirect stdout to that file
    char tempfile[TEMP_FILE_NAME_SIZE];
    snprintf(tempfile, TEMP_FILE_NAME_SIZE, "temp%d", getpid());

    // We continue to loop until no more lines are received from the client.
    while ((command = read_socket_string(connect_socket)) != NULL) {
        worker_pid = fork();

        if (worker_pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (worker_pid == 0) {
            // In the child, redirect all output to the tempfile
            freopen(tempfile, "a", stdout);
            freopen(tempfile, "a", stderr);

            // Execute the provided command
            char** args = tokenize(command);
            int status = execvp(args[0], args);

            // If we got here, command execution failed, so print an error and
            // exit.
            perror("execution error");

            exit(status);
        } else {
            // In the worker process, wait for command execution to complete
            waitpid(worker_pid, NULL, 0);

            printf("Finished running command on worker %d.\n", worker_pid);

            // Read the contents of the tempfile that stored the command output
            // and send it back to the client.
            FILE* fp = fopen(tempfile, "r");

            char c;
            do {
                c = fgetc(fp);
                Socket_putc(c, connect_socket);
            } while (c != EOF);

            fclose(fp);
            remove(tempfile);
        }
    }
}


/**
 * Read a null-terminated string from a socket.
 *
 * The string is dynamically sized based on the number of characters read. The
 * size that the string grows by is determined by the STRING_CHUNK_SIZE
 * constant.
 *
 * Args:
 *     socket:
 *         The socket to read from.
 *
 * Returns:
 *     A string containing each character read from the socket until a null
 *     character ('\0') was found. If the string contains EOF, NULL is returned
 *     instead.
 */
char* read_socket_string(Socket socket) {
    char c;
    char* str = NULL;
    char* tmp = NULL;
    size_t index = 0;
    size_t size = 0;

    do {
        c = Socket_getc(socket);
        if (c == EOF) {
            printf("Failed to get character. Received EOF from socket.\n");
            return NULL;
        }

        // Our string is not big enough, so allocate STRING_CHUNK_SIZE more
        // slots.
        if (index >= size) {
            size += STRING_CHUNK_SIZE;
            tmp = realloc(str, size);

            if (tmp == NULL) {
                printf("Failed to allocate memory for string.");
                exit(EXIT_FAILURE);
            }

            str = tmp;
        }

        str[index] = c;
        index += 1;
    } while(c != '\0');

    return str;
}


/**
 * Tokenize the provided input.
 *
 * This function splits the given line into 'arguments' where each argument is
 * separated by whitespace.
 *
 * Function adapted from the following Stack Overflow answer:
 * https://stackoverflow.com/a/11198630/3762084
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
        tokenized = realloc(tokenized, sizeof(char*) * num_spaces);

        tokenized[num_spaces - 1] = current_token;

        // Pull the next token, if any, from the above 'strtok' call.
        current_token = strtok(NULL, WHITESPACE);
    }

    // After tokenizing the input, we need to add a NULL as the last entry
    tokenized = realloc(tokenized, sizeof(char*) * (num_spaces + 1));
    tokenized[num_spaces] = NULL;

    return tokenized;
}
