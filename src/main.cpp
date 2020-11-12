// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <filesystem>
#include <cstdio>
#include <readline/readline.h>
#include <server.h>

#include "shell.h"
#include "globals.h"
#include "options_parser.h"

int __thread state = 0;

command_line_options parse_arguments(int argc, char **argv);

int main(int argc, char **argv) {
    command_line_options conf;
    conf.parse(argc, argv);
    if (conf.get_filenames().size() > 1) {
        std::cerr << "Error: Too many arguments" << std::endl;
        return EXIT_FAILURE;
    }
    if (conf.get_server()) {
        if (not conf.get_filenames().empty()) {
            std::cerr << "Error: Server do not process script files" << std::endl;
            return EXIT_FAILURE;
        }
        state = 2;
        rl_bind_key('\r', []([[maybe_unused]]int a, [[maybe_unused]] int b) { return 0; });
        start_server(loop, conf.get_port());
    }
    if (not conf.get_filenames().empty()) {
        auto filename = conf.get_filenames()[0];
        if (std::filesystem::path(filename).extension() == ".msh") {
            rl_instream = fopen(filename.data(), "r");
            rl_outstream = rl_instream;
            state = 1;
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