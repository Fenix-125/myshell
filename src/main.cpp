// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <filesystem>
#include <cstdio>
#include "shell.h"
#include "options_parser.h"
#include <readline/readline.h>

command_line_options parse_arguments(int argc, char **argv);

int main(int argc, char **argv) {
    command_line_options conf;
    conf.parse(argc, argv);
    if (conf.get_filenames().size() > 1) {
        std::cerr << "Too many arguments" << std::endl;
        return EXIT_FAILURE;
    }
    if (not conf.get_filenames().empty()) {
        auto filename = conf.get_filenames()[0];
        if (std::filesystem::path(filename).extension() == ".msh") {
            rl_instream = fopen(filename.data(), "r");
            rl_outstream = rl_instream;
            if (rl_instream == nullptr) {
                std::cerr << "File \"" << filename.data() << "\" does not exists. Stopping program" << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            std::cerr << "Bad script extension (.msh required)" << std::endl;
            return EXIT_FAILURE;
        }
    } else {
        rl_instream = stdin;
    }
    loop();
    return EXIT_SUCCESS;
}