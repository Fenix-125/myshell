// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


//
// Created by myralllka on 9/13/20.
//

#include "file_interface/file_interface.h"
#include <cerrno>
#include <cstdlib>
#include <locale>

int write_buffer(int fd, char *buffer, ssize_t size) {
    ssize_t written_bytes = 0;
    ssize_t written_now;

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
    return 0;
}

int processing(int fd_from, int fd_to) {
    ssize_t read_now, read_bytes = 0;
    std::array<char, BUFSIZ> buffer{};
    int res;
    while (true) {
        read_now = read(fd_from, (void *) buffer.data(), BUFSIZ);
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
        res = write_buffer(fd_to, buffer.data(), read_now);
        if (res != 0) {
            return res;
        }
        if (read_now < BUFSIZ) break;
    }
    return 0;
}
