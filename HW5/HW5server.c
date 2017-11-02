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

#include "Socket.h"


ServerSocket welcome_socket;
Socket connect_socket;


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Please provide a port number for the server.\n");
        exit(EXIT_FAILURE);
    }

    welcome_socket = ServerSocket_new(atoi(argv[1]));
    if (welcome_socket < 0) {
        printf("Failed to create welcome socket.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
