// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 9/22/20.
//

// TODO: expand vars
// TODO: GLOB's
// TODO: columns parsing
// TODO: add ./bin to PATH
// TODO: mexport
// TODO: scripts .msh files
// TODO: add merrno

#include <unistd.h>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <wait.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

std::vector<std::string> builtin_str = {
        // TODO: vector of functions
        "mcd",
        "mexit",
        "mpwd",
        "mecho",
        "mexport"
};

int (*builtin_commands[])(std::vector<std::string> &) = {
        &mcd,
        &mexit,
        &mpwd,
        &mecho,
        &mexport
};

bool execute(std::vector<std::string> argv) {
    pid_t pid;
    int status;
    std::vector<const char *> args_for_execvp;

    if (argv.empty()) {
        return EXIT_SUCCESS;
    }

    for (size_t i = 0; i < builtin_str.size(); i++) {
        if (argv[0] == builtin_str[i]) {
            return (*builtin_commands[i])(argv);
        }
    }

    args_for_execvp.reserve(argv.size());
    for (const auto &el:argv) {
        args_for_execvp.push_back(el.c_str());
    }

    args_for_execvp.push_back(nullptr);

    pid = fork();

    if (pid == 0) {
        // Child process
        if (execvp(args_for_execvp[0], const_cast<char *const *>(args_for_execvp.data()))) {
            std::cerr << "error while execvp: " << argv[0] << std::endl;
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        std::cerr << "error while fork" << std::endl;
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return EXIT_SUCCESS;
}

std::string read_line() {
    std::string line;
    std::string prompt = std::filesystem::current_path().string() + " $ ";
    line = readline(prompt.c_str());
    if (line.empty()) {
        if (std::cin.eof()) {
            exit(EXIT_SUCCESS);
        }
        std::cerr << "error while reading line" << std::endl;
        exit(EXIT_FAILURE);
    } else add_history(line.c_str());
    return line;
}

static inline void strip(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
    if (size_t index = s.find('#') != std::string::npos) {
        s.erase(s.begin() + index + 1, s.end());
    }
}

std::vector<std::string> split_line(std::string &line) {
    std::vector<std::string> result;
    boost::split(result, line, boost::is_any_of(" "));
    return result;
}

void loop() {
    std::string line;
    std::vector<std::string> tmp;
//    std::atexit([]() { write_history(".myshell_history"); });
    int status;
    if (std::filesystem::exists(".myshell_history")) {
        read_history(".myshell_history");
    }
    do {
        std::vector<std::string> arguments_for_execv;
        line = read_line();
        strip(line);
        tmp = split_line(line);
        arguments_for_execv.reserve(tmp.size() + 1);
        for (const auto &parameter:tmp) {
            arguments_for_execv.push_back(parameter);
        }
        status = execute(arguments_for_execv);
    } while (status == EXIT_SUCCESS);
    write_history(".myshell_history");
//    exit(EXIT_SUCCESS);
}

int mcd(std::vector<std::string> &argv) {
    if (argv.size() == 1) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(argv[1].c_str()) != 0) {
            std::cout << "error while mcd" << std::endl;
        }
    }
    return EXIT_SUCCESS;
}

int mexit(std::vector<std::string> &argv) {
    if (argv.empty()) {
        exit(EXIT_FAILURE);
    } else if (argv.size() > 2) {
        std::cerr << "mexit: too many arguments" << std::endl;
    }
    exit(std::stoi(argv[1]));
}

int mpwd([[maybe_unused]] std::vector<std::string> &argv) {
    std::cout << std::filesystem::current_path().c_str() << std::endl;
    return EXIT_SUCCESS;
}

int mecho(std::vector<std::string> &argv) {
    for (size_t i = 1; i < argv.size(); ++i) {
        if (!argv[i].empty()) {
            std::cout << argv[i] << " ";
        }
    }
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

int mexport(std::vector<std::string> &argv) {
    // TODO: realize this func!
    return EXIT_SUCCESS;
}
