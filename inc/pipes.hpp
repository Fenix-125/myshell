//
// Created by myralllka on 10/31/20.
//

#ifndef MYSHELL_PIPES_HPP
#define MYSHELL_PIPES_HPP

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

struct pipe_proc_t {
    pipe_proc_t(std::vector<std::string> &&command, pipe_desc_t &&pipe, pipe_state_t &&pipe_state)
            : command(std::move(command)), pipe(pipe), pipe_state(std::move(pipe_state)) {}

    std::vector<std::string> command;
    pipe_desc_t pipe;
    pipe_state_t pipe_state;
};

int execute(std::vector<std::string> &&argv,
            const pipe_state_t &pipe_state = {false, true, true, false, false, redirections{}, 0},
            [[maybe_unused]] const std::pair<int, std::string> &sub_shell_var = {-2, ""});

void kill_pipeline(const std::vector<pipe_proc_t> &pipeline);

int close_all_pipes(const std::vector<pipe_proc_t> &pipeline);

int run_pipeline(std::vector<pipe_proc_t> &&pipeline, const std::pair<bool, std::string> &subs = {});

#endif //MYSHELL_PIPES_HPP
