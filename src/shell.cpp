// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 9/22/20.
//

// TODO: create preprocessor function (strip + expand vars + delete comments started with '#')

#include <unistd.h>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <wait.h>
#include <iostream>
#include <vector>

#include "shell.h"

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char *builtin_str[] = {
        "mcd",
        "mexit",
        "mpwd",
        "mecho",
        "mexport"
};

int (*builtin_commands[])(std::vector<const char *>) = {
        &mcd,
        &mexit,
        &mpwd,
        &mecho,
        &mexport
};

int num_builtin_commands() {
    return sizeof(builtin_str) / sizeof(char *);
}

int launch(std::vector<const char *> argv) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(argv[0], const_cast<char *const *>(argv.data()))) {
            std::cerr << "error while execvp: " << argv[0] << std::endl;
            exit(EXIT_FAILURE);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        std::cerr << "error while fork" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int execute(std::vector<const char *> argv) {
    int i;

    if (argv[0] == nullptr) {
        return 1;
    }

    for (i = 0; i < num_builtin_commands(); i++) {
        if (strcmp(argv[0], builtin_str[i]) == 0) {
            return (*builtin_commands[i])(argv);
        }
    }

    return launch(argv);
}

std::string read_line() {
    std::string line;
    getline(std::cin, line);
    if (line.empty()) {
        std::cerr << "error while reading line" << std::endl;
        exit(EXIT_FAILURE);
    }
    return line;

}

std::vector<std::string> split_line(std::string &line) {
    std::vector<std::string> result;
    boost::split(result, line, boost::is_any_of(" "));
    return result;
}

void lsh_loop() {
    std::string line;
    std::vector<std::string> tmp;

    int status;
    do {
        std::vector<const char *> arguments_for_execv;
        std::cout << std::filesystem::current_path().c_str() << " $ ";
        line = read_line();
        tmp = split_line(line);
        arguments_for_execv.reserve(tmp.size() + 1);
        for (const auto &parameter:tmp) {
            arguments_for_execv.push_back(parameter.c_str());
        }
        arguments_for_execv.push_back(nullptr);
        status = execute(arguments_for_execv);

    } while (status);
}

int mcd(std::vector<const char *> argv) {
    if (argv[1] == nullptr) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(argv[1]) != 0) {
            std::cout << "error while cd" << std::endl;
        }
    }
    return 1;
}

int mexit(std::vector<const char *> argv) {
    if (argv[1] == nullptr) {
        exit(EXIT_SUCCESS);
    }
    exit(atoi(argv[1]));
}

int mpwd(std::vector<const char *> argv) {
    std::cout << std::filesystem::current_path() << std::endl;
    return 1;
}

int mecho(std::vector<const char *> argv) {
    for (int i = 1; i < argv.size(); ++i) {
        if (argv[i] != nullptr) {
            std::cout << argv[i] << " ";
        }
    }
    return EXIT_SUCCESS;
}

int mexport(std::vector<const char *> argv) {

    return EXIT_SUCCESS;
}

/*
shell::shell() {
    char *buff = new char[PATH_MAX];
    auto cwd = getcwd(buff, PATH_MAX);
    pwd = cwd;

    std::string path = std::getenv("PATH");
    path += ":" + pwd;
    setenv("PATH", path.c_str(), 1);

}*/

/*
loop() {
    char *argv[BOOST_FUNCTION_MAX_argv];
    std::cout << pwd << " $ >" << std::endl;
    std::string parameters;
    getline(std::cin, parameters);
    std::vector<std::string> pars;
    boost::split(pars, parameters, boost::is_any_of(" "));

    for (size_t i = 1; i < pars.size(); ++i) {
        argv[i] = pars[i].data();
        std::cout << argv[i] << std::endl;
    }

    execute(pars[0], argv);

    free(argv);
    return EXIT_SUCCESS;
}

execute(const std::string &program_name, char **argv) {
    pid_t pid = fork();
    char **vars = new char *[4096];

    if (pid == -1) {
        std::cerr << "fork failed" << std::endl;
    } else if (pid == 0) {
        auto err = execve(program_name.c_str(), argv, vars);
        std::cout << err << std::endl;
    } else {
        std::cout << "father" << std::endl;
        int status;
        (void) waitpid(pid, &status, 0);
        std::cout << "closed" << std::endl;
    }
    free(vars);
}*/
