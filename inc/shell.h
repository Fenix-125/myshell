//
// Created by myralllka on 9/22/20.
//

#ifndef LAB3_MYSHELL_SHELL_H
#define LAB3_MYSHELL_SHELL_H

#include <map>
#include <vector>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void loop();

std::string read_line();

std::vector<std::string> split_line(std::string &line);

int execute(std::vector<const char *> argv);

bool launch(std::vector<const char *> argv);

int mcd(std::vector<const char *> &argv);

int mexit(std::vector<const char *> &argv);

int mpwd([[maybe_unused]] std::vector<const char *> &argv);

int mecho(std::vector<const char *> &argv);

int mexport(std::vector<const char *> &argv);

#endif //LAB3_MYSHELL_SHELL_H

