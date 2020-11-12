// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 10/31/20.
//

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <boost/program_options.hpp>

#include "globals.h"
#include "commands.hpp"
#include "shell.h"

void matexit() {
    if (state != 2) {
        write_history(nullptr);
    }
    if (state == 2) {
        write_logs("User exited.");
    }
    fclose(rl_instream);
    exit(EXIT_FAILURE); // -V2014
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

namespace po = boost::program_options;

int mpwd(std::vector<std::string> &argv) {
    const auto parsed = pars(argv, "mpwd help");
    if (parsed.help) {
        merrno_val = 0;
        return EXIT_SUCCESS;
    }
    argv = parsed.args;
    if (argv.empty()) {
        return no_arguments("mpwd");
    } else if (argv.size() > 1) {
        return too_many_arguments("mpwd");
    }
    std::cout << std::filesystem::current_path().c_str() << std::endl;
    merrno_val = 0;
    return EXIT_SUCCESS;
}


int mcd(std::vector<std::string> &argv) { // -V2009
    const auto parsed = pars(argv, "mcd help");
    int status;
    if (parsed.help) {
        merrno_val = 0;
        return EXIT_SUCCESS;
    }
    if (argv.empty())
        return no_arguments("mexit");
    if (argv.size() == 1) {
        auto tmp = getenv("HOME");
        if (tmp == nullptr)
            std::cerr << "Error reading HOME variable!" << std::endl;
        else {
            status = chdir(tmp);
            if (status != 0) {
                std::cerr << "Error changing dir!" << std::endl;
            }
        }
    } else {
        if (chdir(argv[1].c_str()) != 0) {
            std::cout << "error while mcd" << std::endl;
            merrno_val = EINVAL;
        } else {
            merrno_val = 0;
        }
    }
    return EXIT_SUCCESS;
}

int mexit(std::vector<std::string> &argv) {
    const auto parsed = pars(argv, "mexit help");
    if (parsed.help) {
        merrno_val = 0;
        return EXIT_SUCCESS;
    }
    argv = parsed.args;
    if (argv.empty()) {
        return no_arguments("mexit");
    } else if (argv.size() > 2) {
        return too_many_arguments("mexit");
    }
    matexit();
    return EXIT_SUCCESS;
}

int mecho(std::vector<std::string> &argv) {
    const auto parsed = pars(argv, "mecho help");
    if (parsed.help) {
        merrno_val = 0;
        return EXIT_SUCCESS;
    }
    argv = parsed.args;
    for (size_t i = 1; i < argv.size(); ++i) {
        if (!argv[i].empty()) {
            std::cout << argv[i] << " ";
        }
    }
    std::cout << std::endl;
    merrno_val = 0;
    return EXIT_SUCCESS;
}

int myexec(std::vector<std::string> &argv) { // -V2009
    if (argv.empty() || argv.size() == 1) {
        return no_arguments(".");
    } else if (argv.size() > 2) {
        return too_many_arguments(".");
    }
    auto filename = argv[1];
    if (std::filesystem::path(filename).extension() == ".msh") {
        rl_instream = fopen(filename.data(), "r");
        rl_outstream = rl_instream;
        if (rl_instream == nullptr) {
            std::cerr << "File \"" << filename.data() << "\" does not exists. Stopping program" << std::endl;
        } else
            launch_loop(true);
    } else {
        std::cerr << "Bad script extension (.msh required)" << std::endl;
    }
    rl_instream = stdin;
    rl_outstream = stdout; // -V519
    return EXIT_SUCCESS;
}

int merrno(std::vector<std::string> &argv) {
    const auto parsed = pars(argv, "merrno help");
    if (parsed.help) {
        merrno_val = 0;
        return EXIT_SUCCESS;
    }
    argv = parsed.args;
    if (argv.empty()) {
        return no_arguments("merrno");
    } else if (argv.size() > 1) {
        return too_many_arguments("merrno");
    }
    std::cout << merrno_val << std::endl;
    merrno_val = 0;
    return EXIT_SUCCESS;
}

parsed_args pars(const std::vector<std::string> &argv, const std::string &help_msg) {
    std::vector<std::string> vector;
    bool help;

    po::options_description visible("Supported options");
    visible.add_options()
            ("help,h", po::value<bool>(&help)->zero_tokens(), "Print this help message.");

    po::options_description hidden("Hidden options");
    hidden.add_options()
            ("args", po::value<std::vector<std::string>>(&vector)->composing());

    po::positional_options_description p;
    p.add("args", -1);

    po::options_description all("All options");
    all.add(visible).add(hidden);

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argv).options(all).positional(p).run(), vm);
    } catch (boost::wrapexcept<po::unknown_option> &_e) {
        std::cerr << "Line parser error: \n" << _e.get_option_name() << std::endl;
    }
    try {
        po::notify(vm);
    } catch (const boost::exception &e) {
        std::cerr << "Error: while parsing parameters";
        exit(1); // -V2014
    }
    if (vm.count("help")) {
        std::cout << help_msg << "\n" << visible << std::endl;
    }
    return parsed_args{static_cast<bool>(vm.count("help")), vector};
}

void write_logs(const std::string &log) {
    char tm_s[100];
    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);
    sprintf(tm_s, "%d-%02d-%02d %02d:%02d:%02d : %s : [L]",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec, user_ip);
    auto status = write(out_fd, tm_s, strlen(tm_s));
    if (status == 0) {
        status = write(out_fd, log.data(), log.size());
    }
    if (status == 0) {
        status = write(out_fd, "\n", 2);
    }
    if (status != 0) {
        return;
    }
    std::cerr << "[myshell] error writing logs" << std::endl;
}