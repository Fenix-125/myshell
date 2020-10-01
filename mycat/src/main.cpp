#include <iostream>
#include "options_parser_mycat.h"
#include "file_interface/file_interface.h"
#include <fcntl.h>
#include <array>

#define  _FILE_OFFSET_BITS 64

const int BUFFER_SIZE = 4096;

command_line_options parse_arguments(int argc, char **argv);

int main(int argc, char **argv) {
    /* ##### Parse program options ##### */
    command_line_options conf = parse_arguments(argc, argv);

    /* ##### Body of the main function ##### */

    std::array<char, BUFFER_SIZE> buf{};
    int res;
    std::vector<int> fd_vector;

    for (auto &filename: conf.get_filenames()) {
        auto fd = open(filename.data(), O_RDONLY);
        if (fd == -1) {
            std::cerr << "File \"" << filename << "\" does not exists. Stopping program" << std::endl;
            return -1;
        }
        fd_vector.emplace_back(fd);
    }

    for (auto &fd:fd_vector) {
        res = processing(fd, buf.data(), BUFFER_SIZE, conf.get_A());
        close(fd);
        if (res != 0) {
            std::cerr << "error while processing: " << res << std::endl;
            return res;
        }
    }

    return 0;
}

command_line_options parse_arguments(int argc, char **argv) {
    //  #####################    Config File Parsing    ######################
    command_line_options config;
    config.parse(argc, argv);
    std::vector<std::string> filenames;
    return config;
}