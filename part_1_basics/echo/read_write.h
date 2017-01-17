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

#ifndef ECHO_READ_WRITE_H
#define ECHO_READ_WRITE_H

#include <unistd.h>
#include <errno.h>

ssize_t read_line_from_socket(int sockd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, *buffer;

    buffer = vptr;

    for (n = 1; n < maxlen; n++) {

        if ((rc = read(sockd, &c, 1)) == 1) {
            *buffer++ = c;
            if (c == '\n')
                break;
        }
        else if (rc == 0) {
            if (n == 1)
                return 0;
            else
                break;
        }
        else {
            if (errno == EINTR)
                continue;
            return -1;
        }
    }

    *buffer = 0;
    return n;
}

ssize_t write_line_to_socket(int sockd, const void *vptr, size_t n) {
    size_t n_left;
    ssize_t n_written;
    const char *buffer;

    buffer = vptr;
    n_left = n;

    while (n_left > 0) {
        if ((n_written = write(sockd, buffer, n_left)) <= 0) {
            if (errno == EINTR)
                n_written = 0;
            else
                return -1;
        }
        n_left -= n_written;
        buffer += n_written;
    }

    return n;
}

#endif //ECHO_READ_WRITE_H
