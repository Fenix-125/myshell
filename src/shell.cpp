//
// Created by myralllka on 9/22/20.
//

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
        "mpwd"
};

int (*builtin_commands[])(char **) = {
        &mcd,
        &mexit,
        &mpwd
};

int num_builtin_commands() {
    return sizeof(builtin_str) / sizeof(char *);
}

int launch(char **argv) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(argv[0], argv) == -1) {
            std::cerr << "error while execvp" << std::endl;
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

int execute(char **argv) {
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
    char **argv = new char *[4096];
    int status;

    do {
        std::cout << std::filesystem::current_path() << " $ ";
        line = read_line();
        auto tmp = split_line(line);
        size_t i;
        for (i = 0; i < tmp.size(); ++i) {
            argv[i] = tmp[i].data();
        }
        argv[i + 1] = nullptr;

        status = execute(argv);

        delete[] argv;
    } while (status);
}

int mcd(char **argv) {
    if (argv[1] == nullptr) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(argv[1]) != 0) {
            std::cout << "error while cd" << std::endl;
        }
    }
    return 1;
}

int mexit(char **argv) {
    exit(EXIT_SUCCESS);
}

int mpwd(char **argv) {
    std::cout << std::filesystem::current_path() << std::endl;
}

//shell::shell() {
//    char *buff = new char[PATH_MAX];
//    auto cwd = getcwd(buff, PATH_MAX);
//    pwd = cwd;
//
//    std::string path = std::getenv("PATH");
//    path += ":" + pwd;
//    setenv("PATH", path.c_str(), 1);
//
//}

//loop() {
//    char *argv[BOOST_FUNCTION_MAX_argv];
//    std::cout << pwd << " $ >" << std::endl;
//    std::string parameters;
//    getline(std::cin, parameters);
//    std::vector<std::string> pars;
//    boost::split(pars, parameters, boost::is_any_of(" "));
//
//    for (size_t i = 1; i < pars.size(); ++i) {
//        argv[i] = pars[i].data();
//        std::cout << argv[i] << std::endl;
//    }
//
//    execute(pars[0], argv);
//
//    free(argv);
//    return EXIT_SUCCESS;
//}
//
//execute(const std::string &program_name, char **argv) {
//    pid_t pid = fork();
//    char **vars = new char *[4096];
//
//    if (pid == -1) {
//        std::cerr << "fork failed" << std::endl;
//    } else if (pid == 0) {
//        auto err = execve(program_name.c_str(), argv, vars);
//        std::cout << err << std::endl;
//    } else {
//        std::cout << "father" << std::endl;
//        int status;
//        (void) waitpid(pid, &status, 0);
//        std::cout << "closed" << std::endl;
//    }
//    free(vars);
//}
