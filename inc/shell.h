//
// Created by myralllka on 9/22/20.
//

#ifndef LAB3_MYSHELL_SHELL_H
#define LAB3_MYSHELL_SHELL_H

#include <map>
#include <vector>

void loop();

std::string read_line(bool internal_func);

std::vector<std::string> split_line(std::string &line);

bool execute(std::vector<std::string> &&argv);

int mcd(std::vector<std::string> &argv);

int mexit(std::vector<std::string> &argv);

int mpwd([[maybe_unused]] std::vector<std::string> &argv);

int mecho(std::vector<std::string> &argv);

int mexport(std::vector<std::string> &argv);

int merrno(std::vector<std::string> &argv);

int myexec(std::vector<std::string> &argv);

#endif //LAB3_MYSHELL_SHELL_H

