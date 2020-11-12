//
// Created by fenix on 01.10.20.
//

#ifndef MYSHELL_GLOBALS_H
#define MYSHELL_GLOBALS_H

#include <time.h>
#include <unistd.h>

extern __thread int merrno_val;

extern __thread int state; // 0 - shell, 1 - script, 2 - server

extern __thread char user_ip[24];

extern int out_fd; // very big life hack =(

void write_logs(const std::string &log);

#endif //MYSHELL_GLOBALS_H
