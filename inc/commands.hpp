//
// Created by myralllka on 10/31/20.
//

#ifndef MYSHELL_COMMANDS_HPP
#define MYSHELL_COMMANDS_HPP

#include <readline/readline.h>
#include <readline/history.h>

struct parsed_args {
    bool help;
    std::vector<std::string> args;
};

parsed_args pars(const std::vector<std::string> &argv, const std::string &help_msg);

void matexit();

int too_many_arguments(std::string const &program_name);

int no_arguments(std::string const &program_name);

int mcd(std::vector<std::string> &argv);

int mexit(std::vector<std::string> &argv);

int mpwd([[maybe_unused]] std::vector<std::string> &argv);

int mecho(std::vector<std::string> &argv);

int merrno(std::vector<std::string> &argv);

int myexec(std::vector<std::string> &argv);

#endif //MYSHELL_COMMANDS_HPP
