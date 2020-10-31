//
// Created by myralllka on 10/31/20.
//

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

#include "merrno.h"
#include "commands.hpp"

void matexit() {
    write_history(".myshell_history");
    fclose(rl_instream);
    exit(EXIT_FAILURE);
}

int too_many_arguments(std::string const &program_name) {
    std::cerr << program_name.data() << ": too many arguments" << std::endl;
    merrno_val = E2BIG;
    return EXIT_FAILURE;
}

int no_arguments(std::string const &program_name) {
    std::cerr << program_name.data() << "no arguments, unexpected error" << std::endl;
    merrno_val = EINVAL;
    return (EXIT_FAILURE);
}