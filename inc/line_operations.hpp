//
// Created by myralllka on 10/31/20.
//

#ifndef MYSHELL_LINE_OPERATIONS_HPP
#define MYSHELL_LINE_OPERATIONS_HPP

#include <vector>
#include <string>
#include "shell.h"
#include "pipes.hpp"

std::vector<std::string> expand_globs(std::vector<std::string> &&args);

std::string read_line(bool internal_func);

void strip(std::string &s);

std::vector<std::string> split_line(std::string &line);

std::string &expand_vars(std::string &line);

bool expand_redirections(std::vector<std::string> &line, redirections &red);

#endif //MYSHELL_LINE_OPERATIONS_HPP
