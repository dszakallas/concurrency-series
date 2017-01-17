/* Copyright 2017 David Szakallas
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "read_write.h"

#define LISTENQ (1024)
#define MAX_LINE (79)
#define ECHO_PORT (3000)

void* handle_socket(void* arg);

int main(int argc, char *argv[]) {
    int list_s;
    int conn_s;
    short int port;
    struct sockaddr_in servaddr;
    char *endptr;
    pthread_t thrd;
    int *args;

    if (argc >= 2) {
        port = strtol(argv[1], &endptr, 0);
        if (*endptr) {
            fprintf(stderr, "invalid port number.\n");
            exit(1);
        }
    } else {
        port = ECHO_PORT;
    }

    if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "error creating listening socket.\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "error calling bind()\n");
        exit(1);
    }

    if (listen(list_s, LISTENQ) < 0) {
        fprintf(stderr, "error calling listen()\n");
        exit(1);
    }

    while (1) {

        /*  Wait for a connection, then accept() it  */
        if ((conn_s = accept(list_s, NULL, NULL)) < 0) {
            fprintf(stderr, "error calling accept()\n");
            exit(1);
        }
        args = malloc(sizeof(int));
        memcpy(args, &conn_s, sizeof(int));
        pthread_create(&thrd, NULL, &handle_socket, args);

    }
}

void* handle_socket(void* arg) {
    int conn_s;
    char buffer[MAX_LINE];
    int bye;

    conn_s = *(int*)arg;

    bye = 0;

    // read from socket and echo back until client says 'bye'
    while (!bye) {
        read_line_from_socket(conn_s, buffer, MAX_LINE - 1);
        printf("%s", buffer);
        if (!strncmp(buffer, "bye\n", MAX_LINE - 1)) {
            printf("client said bye\n");
            bye = 1;
        }
        write_line_to_socket(conn_s, buffer, strlen(buffer));
    }

    if (close(conn_s) < 0) {
        fprintf(stderr, "error calling close()\n");
        free(arg);
        return NULL;
    }
    free(arg);
    return NULL;
}