// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 9/12/20.
//

#include <filesystem>
#include "options_parser.h"
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
            exit(EXIT_SUCCESS); // -V2014
        }
        notify(var_map);
    } catch (std::exception &E) {
        std::cerr << E.what() << std::endl;
        throw OptionsParseException();
    }
}

void command_line_options::init_opt_description() {
    opt_conf.add_options()
            ("help,h", "Show help message")
            ("port,p", po::value<short int>(&port)->default_value(7777)
                    ->implicit_value(7777), "Specify port number of the remote myshell server. Default port 7777. "
                                            "[-s|--server] option is required")
            ("server,s", po::value<bool>(&server)->default_value(false)
                    ->zero_tokens(), "Start remote myshell server on port 7777 or "
                                     "witch is specified by [-p|--port] option");
}

std::string command_line_options::assert_file_exist(const std::string &f_name) {
    if (!std::filesystem::exists(f_name)) {
        throw std::invalid_argument("File " + f_name + " not found!");
    }
    return f_name;
}