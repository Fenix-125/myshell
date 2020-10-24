//
// Created by myralllka on 9/13/20.
//

#include "file_interface/file_interface.h"
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdlib>
#include <locale>

int write_buffer(int fd, char *buffer, ssize_t size, bool A) {
    ssize_t written_bytes = 0;
    ssize_t written_now;
    if (A) {
        while (written_bytes < size) {
            if (!(isprint(*(buffer + written_bytes)) || (isspace(*(buffer + written_bytes))))) {
                char chr = *(buffer + written_bytes), buf[5];
                sprintf(buf, "\\x%02X", 0xFF & chr);
                written_now = write(fd, &buf, 4);
            } else {
                written_now = write(fd, (void *) (buffer + written_bytes), 1);
            }
            if (written_now == -1) {
                if (errno == EINTR)
                    continue;
                else {
                    return -1;
                }
            } else
                written_bytes += 1;

        }
    } else {
        while (written_bytes < size) {
            written_now = write(fd, (void *) (buffer), size - written_bytes);
            if (written_now == -1) {
                if (errno == EINTR)
                    continue;
                else {
                    return -1;
                }
            } else
                written_bytes += written_now;
        }
    }
    return 0;
}

int processing(int fd, char *buffer, ssize_t size, bool A) {
    ssize_t read_now, read_bytes = 0;
    int res;
    while (true) {
        read_now = read(fd, (void *) buffer, size);
        if (read_now == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (read_now == 0) {
            return static_cast<int>(read_now);
        }
        read_bytes += read_now;
        res = write_buffer(1, buffer, read_now, A);
        if (res != 0) {
            return res;
        }
        if (read_now < size) break;
    }
    return 0;
}
