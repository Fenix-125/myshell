//
// Created by myralllka on 9/22/20.
//

#ifndef LAB3_MYSHELL_SHELL_H
#define LAB3_MYSHELL_SHELL_H

#include <map>
#include <unordered_map>
#include <vector>
#include <string>

struct redirections {
    bool redirect_in = false;
    bool redirect_out = false;
    bool redirect_err = false;
    std::string fin{};
    std::string fout{};
    std::string ferr{};
};

struct pipe_desc_t {
    int in, out;
};

struct pipe_state_t {
    bool in_pipe{}, first_pipe{}, last_pipe{}, bg{}, re{};
    redirections red{};
    pid_t pid{};
};

void loop();

void launch_loop(bool internal_func);

std::string read_line(bool internal_func);

//std::vector<std::string> split_line(std::string &line);

bool expand_redirections(std::vector<std::string> &line, redirections &red);

bool expand_subshell(const std::string &line);

int mexport(std::vector<std::string> &argv);

#endif //LAB3_MYSHELL_SHELL_H

