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

#include "Socket.h"


int main(int argc, char* argv[]) {
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

    Socket_close(connect_socket);

    return 0;
}
