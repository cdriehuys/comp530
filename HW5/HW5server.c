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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Socket.h"


#define STRING_CHUNK_SIZE 32


ServerSocket welcome_socket;
Socket connect_socket;


void exec_command_service();
char* read_socket_string();


int main(int argc, char* argv[]) {
    int child_status;
    pid_t service_pid;

    if (argc < 2) {
        printf("Please provide a port number for the server.\n");
        exit(EXIT_FAILURE);
    }

    welcome_socket = ServerSocket_new(atoi(argv[1]));
    if (welcome_socket < 0) {
        printf("Failed to create welcome socket.\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        connect_socket = ServerSocket_accept(welcome_socket);
        if (connect_socket < 0) {
            printf("Failed to accept connection to server socket.\n");
            exit(EXIT_FAILURE);
        }

        service_pid = fork();

        if (service_pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (service_pid == 0) {
            exec_command_service();
            Socket_close(connect_socket);
            exit(EXIT_SUCCESS);
        } else {
            Socket_close(connect_socket);
            waitpid(-1, &child_status, WNOHANG);
        }
    }

    return 0;
}


void exec_command_service() {
    char* command;

    // The child process doesn't need the welcoming socket
    Socket_close(welcome_socket);

    while ((command = read_socket_string(connect_socket)) != NULL) {
        printf("Read command: %s\n", command);
    }
}


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
