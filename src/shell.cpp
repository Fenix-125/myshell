//
// Created by myralllka on 9/22/20.
//

#include <unistd.h>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <wait.h>
#include <iostream>

#include "shell.h"

shell::shell() {
    char *buff = new char[PATH_MAX];
    auto cwd = getcwd(buff, PATH_MAX);
    pwd = cwd;

    std::string path = std::getenv("PATH");
    path += ":" + pwd;
    setenv("PATH", path.c_str(), 1);
    delete buff;
}

shell::~shell() {
}

int shell::loop() {
    char *argv[BOOST_FUNCTION_MAX_ARGS];
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

int shell::execute(const std::string &program_name, char **argv) {
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
}

int shell::update_history() {

}