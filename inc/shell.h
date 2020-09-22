//
// Created by myralllka on 9/22/20.
//

#ifndef LAB3_MYSHELL_SHELL_H
#define LAB3_MYSHELL_SHELL_H

#include <map>
#include <vector>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void lsh_loop();

std::string read_line();

std::vector<std::string> split_line(const std::string &line);

int launch(char **args);

int mcd(char **args);

int mexit(char **args);

int num_builtin_commands();

int execute(char **args);

#endif //LAB3_MYSHELL_SHELL_H

