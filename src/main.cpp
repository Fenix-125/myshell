// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <filesystem>
#include "shell.h"
#include "options_parser.h"

command_line_options parse_arguments(int argc, char **argv);

int main(int argc, char **argv) {
    command_line_options conf;
    conf.parse(argc, argv);
    if (conf.get_filenames().size() > 1) {
        std::cerr << "too many arguments" << std::endl;
    }
    if (conf.get_filenames().empty()) {
        loop();
    } else if (std::filesystem::path(conf.get_filenames()[0]).extension() == ".msh") {
        //TODO: launch script here
    } else {
        std::cerr << "Bad script extension (.msh required)" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}