//
// Created by fenix on 01.10.20.
//

#ifndef MYSHELL_GLOBALS_H
#define MYSHELL_GLOBALS_H

extern __thread int merrno_val;

extern __thread int state; // 0 - shell, 1 - script, 2 - server

#endif //MYSHELL_GLOBALS_H
