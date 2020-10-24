// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <filesystem>
#include "options_parser_mycp.h"
#include "file_interface/file_interface_mycp.h"

command_line_options parse_arguments(int argc, char **argv);

int main(int argc, char **argv) {
    command_line_options conf = parse_arguments(argc, argv);
    if (conf.get_filenames().empty()) {
        std::cerr << "mycp: error, no parameters specified" << std::endl;
    }
    auto parameters = conf.get_filenames();
    auto destination = parameters.back();
    parameters.pop_back();
    int merrno;
    auto R = conf.get_R();
    ALL = conf.get_f();

    if (std::filesystem::is_directory(destination)) {
        for (const auto &file_entry:parameters) {
            if (not std::filesystem::exists(file_entry)) {
                std::cerr << "mycp: error, file or directory \"" << file_entry << "\" does not exists" << std::endl;
                continue;
            }
            if (R and std::filesystem::is_directory(file_entry)) {
                std::filesystem::create_directory(destination + "/" + file_entry);
                for (auto &directory_entry: std::filesystem::recursive_directory_iterator(file_entry)) {
                    if (directory_entry.is_directory()) {
                        std::filesystem::create_directory(destination + "/" + directory_entry.path().string());
                    } else {
                        merrno = copy_file(file_entry, destination + "/" + directory_entry.path().string());
                        if (merrno != 0) {
                            std::cerr << "mycp: error while copying" << std::endl;
                            return merrno;
                        }
                    }
                }
            } else if (std::filesystem::is_directory(file_entry)) {
                std::cerr << "mycp: -R not specified; omitting directory: \"" << file_entry << std::endl;
                return -1;
            } else {
                merrno = copy_file(file_entry, destination + "/" + file_entry);
                if (merrno != 0) {
                    std::cerr << "mycp: error while copying" << std::endl;
                    return merrno;
                }
            }
        }
    } else {
        if (parameters.size() == 1) {
            copy_file(parameters[0], basename(destination.data()));
        } else {
            std::cerr << "mycp: target \"" << basename(destination.data()) << "\" is not a directory" << std::endl;
            return -1;
        }
    }
    return 0;
}

command_line_options parse_arguments(int argc, char **argv) {
    command_line_options config;
    config.parse(argc, argv);
    std::vector<std::string> filenames;
    return config;
}


