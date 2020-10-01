//
// Created by myralllka on 9/13/20.
//

#ifndef MYCAT_FILE_INTERFACE_H
#define MYCAT_FILE_INTERFACE_H

#include <cstring>
#include <malloc.h>
#include "unistd.h"

int write_buffer(int fd, char *buffer, ssize_t size, bool A);

int processing(int fd, char *buffer, ssize_t size, bool A);

#endif //MYCAT_FILE_INTERFACE_H
