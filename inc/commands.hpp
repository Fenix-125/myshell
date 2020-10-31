//
// Created by myralllka on 10/31/20.
//

#ifndef MYSHELL_COMMANDS_HPP
#define MYSHELL_COMMANDS_HPP

#include <readline/readline.h>
#include <readline/history.h>

void matexit();

int too_many_arguments(std::string const &program_name);

int no_arguments(std::string const &program_name);

#endif //MYSHELL_COMMANDS_HPP
