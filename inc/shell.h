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

std::vector<std::string> split_line(std::string &line);

int num_builtin_commands();

int execute(std::vector<char *> argv);

bool launch(std::vector<const char *> argv);

int mcd(std::vector<const char *> argv);

int mexit(std::vector<const char *> argv);

int mpwd([[maybe_unused]] const std::vector<const char *>& argv);

int mecho(char **argv);

#endif //LAB3_MYSHELL_SHELL_H

