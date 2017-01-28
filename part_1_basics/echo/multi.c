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

struct handler_args_t {
    int conn_s;
    struct sockaddr_in clientaddr;
};

int handle_socket(int conn_s);
void* handle_socket_wrapper(void* arg);

int main(int argc, char *argv[]) {
    int list_s;
    int conn_s;
    short int port;
    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len;
    char *endptr;
    pthread_t thrd;
    struct handler_args_t *args;

    clientaddr_len = sizeof(struct sockaddr_in);

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
        if ((conn_s = accept(list_s, (struct sockaddr *)&clientaddr, &clientaddr_len)) < 0) {
            fprintf(stderr, "error calling accept()\n");
            exit(1);
        }
        printf("%d: accept %s:%hu\n", conn_s, inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
        args = malloc(sizeof(struct handler_args_t));
        args->conn_s = conn_s;
        args->clientaddr = clientaddr;
        pthread_create(&thrd, NULL, &handle_socket_wrapper, args);
    }
}

void* handle_socket_wrapper(void* arg) {
    struct handler_args_t *handler_args;
    handler_args = (struct handler_args_t*)arg;
    if(handle_socket(handler_args->conn_s)) {
        exit(1);
    }
    free(arg);
    return NULL;
}

int handle_socket(int conn_s) {
    char buffer[MAX_LINE];
    // read from socket and echo back until client says 'bye'
    while (1) {
        if(read_line_from_socket(conn_s, buffer, MAX_LINE - 1) > 0) {
            if (!strncmp(buffer, "bye\n", MAX_LINE - 1)) {
                printf("%d: bye\n", conn_s);
                break;
            }
            printf("%d: echo %s", conn_s, buffer);
            write_line_to_socket(conn_s, buffer, strlen(buffer));
        } else {
            break;
        }
    }

    if (close(conn_s) < 0) {
        fprintf(stderr, "error calling close()\n");
        return 1;
    }

    printf("%d: close\n", conn_s);
    return 0;
}