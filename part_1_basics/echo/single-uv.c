/*
Copyright 2014 Thorsten Lorenz, 2017 David Szakallas.
All rights reserved.

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
        files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
        OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 */


/*
 * Mainly rewritten from https://github.com/thlorenz/learnuv/blob/master/exercises/07_tcp_echo_server/solution.c
 *
 * There are other great uv examples there, I recommend you check it out, if you are interested!
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "uv.h"

#define ECHO_PORT (3000)

// leaks resources, but how cares? we're gonna shutdown anyways
#define FATAL_UV_ERR(op, mess) if((uv_err = (op))) {                \
    fprintf(stderr, "fatal: %s: %s\n", (mess), uv_strerror(uv_err));\
    exit(1);                                                        \
}
#define FATAL_NULL(op, mess) if(!(op)) {                            \
    fprintf(stderr, "fatal: %s\n", (mess));                         \
    exit(1);                                                        \
}
#define FATAL_NOT_NULL(op, mess) if((op)) {                         \
    fprintf(stderr, "fatal: %s\n", (mess));                         \
    exit(1);                                                        \
}

#define ERROR_UV_ERR(op, mess, handler) if((uv_err = (op))) {       \
    fprintf(stderr, "error: %s: %s\n", (mess), uv_strerror(uv_err));\
    handler                                                         \
}

struct write_ctx_t {
    uv_stream_t *client;
    uv_buf_t buf;
};

void handle_socket(uv_stream_t *server, int status);
void handle_shutdown(uv_shutdown_t* req, int status);
void handle_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf);
void handle_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
void handle_write(uv_write_t *req, int status);
void handle_close(uv_handle_t* client);

int main(int argc, char *argv[]) {
    uv_loop_t *loop;
    uv_tcp_t tcp;
    int uv_err;
    short int port;
    char *endptr;
    struct sockaddr_in addr;

    if (argc >= 2) {
        port = strtol(argv[1], &endptr, 0);
        FATAL_NOT_NULL(*endptr, "invalid port number")
    } else {
        port = ECHO_PORT;
    }

    uv_err = 0;

    FATAL_NULL(loop = uv_default_loop(), "create uv loop")

    FATAL_UV_ERR(uv_tcp_init(loop, &tcp), "init tcp server")
    FATAL_UV_ERR(uv_ip4_addr("0.0.0.0", port, &addr), "create ip4 address")
    FATAL_UV_ERR(uv_tcp_bind(&tcp, (struct sockaddr*) &addr, AF_INET), "bind socket")
    FATAL_UV_ERR(uv_listen((uv_stream_t*) &tcp, SOMAXCONN, handle_socket), "listen socket")

    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}

void handle_socket(uv_stream_t *server, int status) {
    int uv_err;
    uv_shutdown_t *shutdown_req;
    uv_tcp_t *client;

    uv_err = 0;

    FATAL_UV_ERR(status, "handle socket")

    /* Init client connection using `server->loop`, passing the client handle */
    client = malloc(sizeof(uv_tcp_t));
    FATAL_UV_ERR(uv_tcp_init(server->loop, client), "init tcp server")

    /* Accept the now initialized client connection */
    ERROR_UV_ERR(uv_accept(server, (uv_stream_t*) client), "accept connection", {
        shutdown_req = malloc(sizeof(uv_shutdown_t));
        shutdown_req->data = client;
        FATAL_UV_ERR(uv_shutdown(shutdown_req, (uv_stream_t *) client, handle_shutdown), "shutdown connection")
    })

    /* Start reading data from client */
    ERROR_UV_ERR(uv_read_start((uv_stream_t*) client, handle_alloc, handle_read), "read", {
        shutdown_req = malloc(sizeof(uv_shutdown_t));
        shutdown_req->data = client;
        FATAL_UV_ERR(uv_shutdown(shutdown_req, (uv_stream_t *) client, handle_shutdown), "shutdown connection")
    })
}

void handle_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
    buf->base = malloc(size);
    buf->len = size;
}

void handle_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) {
    int uv_err;
    uv_shutdown_t *shutdown_req;
    uv_write_t *write_req;
    struct write_ctx_t *write_ctx;

    /* Errors or EOF */
    if (nread < 0) {
        if (nread != UV_EOF) {
            fprintf(stderr, "error: reading from socket\n");
        }

        /* Client signaled that all data has been sent, so we can close the connection and are done */
        free(buf->base);
        shutdown_req = malloc(sizeof(uv_shutdown_t));
        shutdown_req->data = client;
        FATAL_UV_ERR(uv_shutdown(shutdown_req, (uv_stream_t *) client, handle_shutdown), "shutdown connection")
        return;
    }

    if (nread == 0) {
        /* Everything OK, but nothing read and thus we don't write anything */
        free(buf->base);
        return;
    }

    /* Check if we should shutdown connection which the client signals by sending "bye" */
    if (!strncmp("bye", buf->base, fmin(nread, 3))) {
        free(buf->base);
        shutdown_req = malloc(sizeof(uv_shutdown_t));
        shutdown_req->data = client;
        FATAL_UV_ERR(uv_shutdown(shutdown_req, (uv_stream_t *) client, handle_shutdown), "shutdown connection")
        return;
    }

    write_req = malloc(sizeof(uv_write_t));

    write_ctx = malloc(sizeof(struct write_ctx_t));
    write_ctx->client = client;
    write_ctx->buf = uv_buf_init(buf->base, nread);

    write_req->data = write_ctx;

    ERROR_UV_ERR(uv_write(write_req, client, &write_ctx->buf, 1, handle_write), "write", {
        fprintf(stderr, "error: writing: %s\n", uv_strerror(uv_err));
        free(write_ctx);
        free(write_req);
        free(buf->base);
        shutdown_req = malloc(sizeof(uv_shutdown_t));
        shutdown_req->data = client;
        FATAL_UV_ERR(uv_shutdown(shutdown_req, (uv_stream_t *) client, handle_shutdown), "shutdown connection")
    })
}

void handle_write(uv_write_t *req, int status) {
    int uv_err;
    struct write_ctx_t *ctx;
    uv_stream_t *client;
    uv_shutdown_t *shutdown_req;

    ctx = req->data;
    client = ctx->client;

    free(ctx->buf.base);
    free(ctx);
    free(req);

    ERROR_UV_ERR(status, "write", {
        shutdown_req = malloc(sizeof(uv_shutdown_t));
        shutdown_req->data = client;
        FATAL_UV_ERR(uv_shutdown(shutdown_req, client, handle_shutdown), "shutdown connection")
    })
}


void handle_shutdown(uv_shutdown_t* req, int status) {
    int uv_err;
    uv_err = 0;
    FATAL_UV_ERR(status, "handle shutdown")
    uv_close((uv_handle_t*) req->handle, handle_close);
    free(req);
}

void handle_close(uv_handle_t* client) {
    free(client);
}
