// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 9/22/20.
//

// TODO: strip
// TODO: expand vars
// TODO: delete comments started with '#'

#include <unistd.h>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <wait.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

#include "shell.h"

std::vector<std::string> builtin_str = {
        "mcd",
        "mexit",
        "mpwd",
        "mecho",
        "mexport"
};

int (*builtin_commands[])(std::vector<const char *> &) = {
        &mcd,
        &mexit,
        &mpwd,
        &mecho,
        &mexport
};


bool launch(std::vector<const char *> argv) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(argv[0], const_cast<char *const *>(argv.data()))) {
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
    return true;
}

int execute(std::vector<const char *> argv) {
    if (argv[0] == nullptr) {
        return EXIT_FAILURE;
    }
    for (size_t i = 0; i < builtin_str.size(); i++) {
        if (strcmp(argv[0], builtin_str[i].data()) == 0) {
            return (*builtin_commands[i])(argv);
        }
    }

    return launch(argv);
}

std::string read_line() {
    std::string line;
    getline(std::cin, line);
    if (line.empty()) {
        if (std::cin.eof()) {
            exit(EXIT_SUCCESS);
        }
        std::cerr << "error while reading line" << std::endl;
        exit(EXIT_FAILURE);
    }
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

    int status;
    do {
        std::vector<const char *> arguments_for_execv;
        std::cout << std::filesystem::current_path().c_str() << " $ ";
        line = read_line();
        strip(line);
        tmp = split_line(line);
        arguments_for_execv.reserve(tmp.size() + 1);
        for (const auto &parameter:tmp) {
            arguments_for_execv.push_back(parameter.c_str());
//            std::cerr << parameter.c_str() << std::endl;
        }
        arguments_for_execv.push_back(nullptr);
        status = execute(arguments_for_execv);

    } while (status);
}

int mcd(std::vector<const char *> &argv) {
    if (argv[1] == nullptr) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(argv[1]) != 0) {
            std::cout << "error while cd" << std::endl;
        }
    }
    return 1;
}

int mexit(std::vector<const char *> &argv) {
    if (argv[1] == nullptr) {
        exit(EXIT_SUCCESS);
    }
    exit(atoi(argv[1]));
}

int mpwd([[maybe_unused]] std::vector<const char *> &argv) {
    std::cout << std::filesystem::current_path() << std::endl;
    return 1;
}

int mecho(std::vector<const char *> &argv) {
    for (size_t i = 1; i < argv.size(); ++i) {
        if (argv[i] != nullptr) {
            std::cout << argv[i] << " ";
        }
    }
    return 1;
}

int mexport(std::vector<const char *> &argv) {
    // TODO: realize this func!
    return 1;
}
