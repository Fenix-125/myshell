// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


//
// Created by myralllka on 9/12/20.
//

#include <filesystem>
#include "options_parser_mycp.h"
#include "exceptions/parser_exeption.h"

namespace po = boost::program_options;

command_line_options::command_line_options() {
    init_opt_description();
}

void command_line_options::parse(int ac, char **av) {
    try {
        po::parsed_options parsed = po::command_line_parser(ac, av).options(opt_conf).allow_unregistered().run();
        store(parsed, var_map);
        filenames = collect_unrecognized(parsed.options, po::include_positional);
        if (var_map.count("help")) {
            std::cout << opt_conf << "\n";
            exit(EXIT_SUCCESS);
        }
        f = var_map.count("force");
        R = var_map.count("recursive");
        notify(var_map);
    } catch (std::exception &E) {
        std::cerr << E.what() << std::endl;
        throw OptionsParseException();
    }
}

void command_line_options::init_opt_description() {
    opt_conf.add_options()
            ("help,h", "Show help message")
            ("force,f", "Don`t ask before rewrite existing files")
            ("recursive,R", "Copy recursively (include directories)");
}