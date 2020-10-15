//
// Created by myralllka on 9/13/20.
//

#ifndef MYCAT_FILE_INTERFACE_H
#define MYCAT_FILE_INTERFACE_H

#include <string>
#include <cstring>
#include <malloc.h>
#include "unistd.h"

extern bool ALL;

int processing(int fd_from, int fd_to);

int copy_file(const std::string &f_from, const std::string &f_to);

#endif //MYCAT_FILE_INTERFACE_H
