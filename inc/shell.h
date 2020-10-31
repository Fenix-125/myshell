//
// Created by myralllka on 9/22/20.
//

#ifndef LAB3_MYSHELL_SHELL_H
#define LAB3_MYSHELL_SHELL_H

#include <map>
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

void loop();

std::string read_line(bool internal_func);

std::vector<std::string> split_line(std::string &line);

struct pipe_state_t {
    bool in_pipe{}, first_pipe{}, last_pipe{}, bg{}, re{};
    redirections red{};
    pid_t pid{};
};

int execute(std::vector<std::string> &&argv, const pipe_state_t &pipe_state,
            const std::pair<bool, std::string> &sub_shell_var);

bool expand_redirections(std::vector<std::string> &line, redirections &red);

bool expand_subshell(const std::string &line);

int mcd(std::vector<std::string> &argv);

int mexit(std::vector<std::string> &argv);

int mpwd([[maybe_unused]] std::vector<std::string> &argv);

int mecho(std::vector<std::string> &argv);

int mexport(std::vector<std::string> &argv);

int merrno(std::vector<std::string> &argv);

int myexec(std::vector<std::string> &argv);

#endif //LAB3_MYSHELL_SHELL_H

