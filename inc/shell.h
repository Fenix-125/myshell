//
// Created by myralllka on 9/22/20.
//

#ifndef LAB3_MYSHELL_SHELL_H
#define LAB3_MYSHELL_SHELL_H

#include <map>
#include <unordered_map>
#include <vector>
#include <string>

void loop();

void launch_loop(bool internal_func);

bool expand_subshell(const std::string &line);

int mexport(std::vector<std::string> &argv);

#endif //LAB3_MYSHELL_SHELL_H

