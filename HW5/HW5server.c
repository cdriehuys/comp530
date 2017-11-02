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

#include "Socket.h"


ServerSocket welcome_socket;
Socket connect_socket;


void exec_command_service();


int main(int argc, char* argv[]) {
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
            waitpid(-1, &chld_status, WNOHANG);
        }
    }

    return 0;
}


void exec_command_service() {
    printf("Would execute command in child process.\n");
}
