// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include <iostream>
#include "options_parser_mycp.h"
#include "file_interface/file_interface.h"
#include <fcntl.h>
#include <array>
#include <filesystem>

const int BUFFER_SIZE = 4096;

command_line_options parse_arguments(int argc, char **argv);

int options_processing(command_line_options const &conf);

int main(int argc, char **argv) {

    command_line_options conf = parse_arguments(argc, argv);

    if (!conf.get_R() and !conf.get_f()) {
        if (conf.get_filenames().size() != 2) {
            std::cerr << "mycp: wrong number of arguments" << std::endl;
            return -1;
        }
        if (std::filesystem::exists(conf.get_filenames()[1])) {
            switch(options_processing(conf)) {
                case 1:
                    // NO
                    std::cout << "Can not copy" << std::endl;
                    return 0;
                case 0:
                    // YES
                case 2:
                    // ALL
                    break;
                case 3:
                    // CANCEL
                    std::cerr << "Canceled" << std::endl;
                    return 0;
                case -1:
                    break;
                default:
                    std::cerr << "unexpected error while options processing"  << std::endl;
                    return -1;
            }
        }
//        else if ()
        auto from = open(conf.get_filenames()[0].c_str(), O_RDONLY);
        auto to = open(conf.get_filenames()[1].c_str(), O_WRONLY | O_CREAT);
        processing(from, to);
    }

    //        auto files = conf.get_filenames();
//        auto destination = files.back();
//        files.pop_back();

    return 0;
}

command_line_options parse_arguments(int argc, char **argv) {
    //  #####################    Config File Parsing    ######################
    command_line_options config;
    config.parse(argc, argv);
    std::vector<std::string> filenames;
    return config;
}

int options_processing(command_line_options const &conf) {
    std::string answer{};
    std::cout << "The file " << conf.get_filenames()[1]
              << " already exists. Do you want to replace it? Y[es]/N[o]/A[ll]/C[ancel] > ";
    if (!std::getline(std::cin, answer)) {
        std::cerr << "Error while reading from command line" << std::endl;
    }
    if (answer == "Y" or answer == "Yes") {
        return 0;
    } else if (answer == "N" or answer == "No") {
        return 1;
    } else if (answer == "A" or answer == "All") {
        return 2;
    } else if (answer == "C" or answer == "Cancel") {
        return 3;
    } else std::cerr << "wrong input" << std::endl;
    return -1;
}
