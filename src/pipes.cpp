// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 10/31/20.
//

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <wait.h>
#include <map>
#include <utility>
#include <boost/function.hpp>

#include "pipes.hpp"
#include "commands.hpp"
#include "shell.h"
#include "globals.h"

const std::map<std::string, std::function<int(std::vector<std::string> &)>> inner_commands = {
        {"mcd",     mcd},
        {"mexit",   mexit},
        {"mpwd",    mpwd},
        {"mecho",   mecho},
        {"mexport", mexport},
        {"merrno",  merrno},
        {".",       myexec}
};

void kill_pipeline(const std::vector<pipe_proc_t> &pipeline) {
    for (auto &pipe_el : pipeline) {
        if (kill(pipe_el.pipe_state.pid, SIGTERM) == -1) {
            std::cerr << "Error: fail to kill '"
                      << pipe_el.command[0]
                      << "' (pid=" << pipe_el.pipe_state.pid << ")"
                      << std::endl;
        } // TODO: check if is valid to kill here
    }
}

int close_all_pipes(const std::vector<pipe_proc_t> &pipeline) {
    int exit_status = EXIT_SUCCESS;
    for (const auto &pipe_el : pipeline) {
        if (pipe_el.pipe.in > 2)
            if (close(pipe_el.pipe.in) == -1)
                exit_status = EXIT_FAILURE;
        if (pipe_el.pipe.out > 2)
            if (close(pipe_el.pipe.out) == -1)
                exit_status = EXIT_FAILURE;
    }
    return exit_status;
}

int run_pipeline(std::vector<pipe_proc_t> &&pipeline, const std::pair<bool, std::string> &subs) {
    if (pipeline.empty()) {
        return EXIT_SUCCESS;
    }
    bool in_parent;
    int status;
    pipe_desc_t subs_pipe_fd{};
    if (subs.first) {
        int sub_pipe[2];
        if (pipe(sub_pipe) == -1) {
            std::cerr << "Error: could not create subshell pipe!" << std::endl;
        }
        subs_pipe_fd.in = sub_pipe[0];
        subs_pipe_fd.out = sub_pipe[1];
    }

    for (auto &pipe_el : pipeline) {
        if (pipe_el.command.empty())
            return EXIT_SUCCESS;
        if ((!pipe_el.pipe_state.first_pipe && pipe_el.pipe_state.red.redirect_in) ||
            (!pipe_el.pipe_state.last_pipe && pipe_el.pipe_state.red.redirect_out) ||
            (pipe_el.pipe_state.last_pipe && pipe_el.pipe_state.red.redirect_out && subs.first)) {
            std::cerr << "Error: bad redirect in pipeline" << std::endl;
            return EXIT_SUCCESS;
        }
        for (auto &command : inner_commands) {
            if (command.first == pipe_el.command[0]) {
                status = command.second(pipe_el.command);
                if (status != 0) {
                    merrno_val = status;
                    matexit();
                }
                return EXIT_SUCCESS;
            }
        }
        switch (pipe_el.pipe_state.pid = fork()) {
            case -1:
                std::cerr << "Error: fail to fork" << std::endl;
                return EXIT_SUCCESS;
            case 0:             /* First child: exec 'ls' to write to pipe */
                in_parent = false;
                break;
            default:            /* Parent falls through to create next child */
                in_parent = true;
                break;
        }
        if (!in_parent) {
            if (pipe_el.pipe.in != STDIN_FILENO)
                if (dup2(pipe_el.pipe.in, STDIN_FILENO) == -1)
                    std::cerr << "dup2 in" << std::endl;
            if (pipe_el.pipe.out != STDOUT_FILENO)
                if (dup2(pipe_el.pipe.out, STDOUT_FILENO) == -1)
                    std::cerr << "dup2 out" << std::endl;
            if (close_all_pipes(pipeline) == EXIT_FAILURE) {
                std::cerr << "Error: while closing pipes in child!" << std::endl;
            }
            if (subs.first && pipe_el.pipe_state.last_pipe) {
                if (dup2(subs_pipe_fd.out, STDOUT_FILENO) == -1) {
                    std::cerr << "dup2 sub" << std::endl;
                    return EXIT_SUCCESS;
                }
                if (close(subs_pipe_fd.in) == -1 || close(subs_pipe_fd.out) == -1) {
                    std::cerr << "close sub in child" << std::endl;
                    return EXIT_SUCCESS;
                }
            }
            status = execute(std::move(pipe_el.command), pipe_el.pipe_state);
            std::cerr << "Error: fail to execute in child" << std::endl;
            return EXIT_FAILURE;
        } else if (subs.first && pipe_el.pipe_state.last_pipe) {
            if (close(subs_pipe_fd.out) == -1) {
                std::cerr << "close sub in child" << std::endl;
                return EXIT_SUCCESS;
            }
            if (close_all_pipes(pipeline) == EXIT_FAILURE) {
                std::cerr << "Error: while closing pipes in parent" << std::endl;
                kill_pipeline(pipeline); // TODO: review
                return EXIT_SUCCESS;
            }
            status = execute(std::move(pipe_el.command), pipe_el.pipe_state,
                             std::pair<int, std::string>{subs_pipe_fd.in, subs.second});
        } else {
            status = execute(std::move(pipe_el.command), pipe_el.pipe_state);
        }
        if (status != EXIT_SUCCESS) {
            kill_pipeline(pipeline);
            return EXIT_SUCCESS;
        }
    }
    /* Parent closes unused file descriptors for pipe, and waits for children */
    if (!subs.first)
        if (close_all_pipes(pipeline) == EXIT_FAILURE) {
            std::cerr << "Error: while closing pipes in parent" << std::endl;
            kill_pipeline(pipeline); // TODO: review is it needed
            return EXIT_SUCCESS;
        }
    for (auto &pipe_el : pipeline) {
        do {
            waitpid(pipe_el.pipe_state.pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        if (status == -1) {
            std::cerr << "Error: fail to wait pipeline element with status: " << status << std::endl;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}
