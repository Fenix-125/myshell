// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 9/22/20.
//

#include <unistd.h>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <wait.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <readline/readline.h>
#include <readline/history.h>
#include <merrno.h> // extern merrno_val
#include <boost/program_options.hpp>
#include "shell.h"
#include <glob_posix.h>
__thread int merrno_val = 0;

std::unordered_map<std::string, std::string> global_var_map{};

void matexit() {
    write_history(".myshell_history");
    fclose(rl_instream);
    exit(EXIT_FAILURE);
}

bool execute(std::vector<std::string> &&argv) {
    pid_t pid;
    int status;

    std::map<std::string, std::function<int(std::vector<std::string> &)>> inner_commands = {
            {"mcd",     mcd},
            {"mexit",   mexit},
            {"mpwd",    mpwd},
            {"mecho",   mecho},
            {"mexport", mexport},
            {"merrno",  merrno},
            {".",       myexec}
    };

    if (argv.empty()) {
        return EXIT_SUCCESS;
    }

    for (auto &command:inner_commands) {
        if (command.first == argv[0]) {
            return command.second(argv);
        }
    }

    std::vector<const char *> args_for_execvp;
    args_for_execvp.reserve(argv.size());
    for (const auto &el : argv) {
        args_for_execvp.emplace_back(el.c_str());
    }
    args_for_execvp.emplace_back(nullptr);

    pid = fork();

    if (pid == 0) {
        // Child process
        if (execvp(args_for_execvp[0], const_cast<char *const *>(args_for_execvp.data()))) {
            if (std::filesystem::path(args_for_execvp[0]).extension() == ".msh") {
                myexec(argv);
            }
            std::cerr << "error while execvp: " << argv[0] << std::endl;
            return EXIT_FAILURE;
        }
    } else if (pid < 0) {
        // Error forking
        std::cerr << "error while fork" << std::endl;
        merrno_val = ECHILD;
        return EXIT_SUCCESS;
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    merrno_val = 0;
    return EXIT_SUCCESS;
}

std::string read_line(bool internal_func) {
    char *line;
    std::string prompt = std::filesystem::current_path().string() + " $ ";
    if (fileno(rl_instream) == 0) {
        line = readline(prompt.c_str());
        if (line[0] != '\0')
            add_history(line);
    } else line = readline("");
    if (line == nullptr) {
        if (internal_func)
            return "\0";
        else
            matexit();
    }
    return line;
}

static inline auto iterall_effect_pos(const std::string &line, const std::string &token, size_t offset = 0) {
    std::string::size_type effected_word_start = line.find(token, offset);
    if (effected_word_start == std::string::npos)
        return std::pair<std::string::size_type, std::string::size_type>{-1, -1};
    else
        effected_word_start += token.size();
    const std::string::size_type effected_end =
            std::find_if(line.begin() + effected_word_start, line.end(),
                         [](const char &ch) {
                             return std::isspace(ch);
                         }) - line.begin();
    return std::pair<std::string::size_type, std::string::size_type>{effected_word_start, effected_end};
}

static inline std::string &expand_vars(std::string &line) {
    std::stringstream s{};
    std::string name;
    const std::string token = "$";
    size_t offset = 0u;
    auto effect_pos = iterall_effect_pos(line, token);
    while (effect_pos.first != std::string::npos) {
        s << line.substr(offset, (effect_pos.first - token.size()) - offset);
        name = line.substr(effect_pos.first, effect_pos.second - effect_pos.first);
        if (global_var_map.find(name) != global_var_map.end()) {
            s << global_var_map[name];
        }
        offset = effect_pos.second;
        effect_pos = iterall_effect_pos(line, token, offset); // add offset
    }
    s << line.substr(offset);
    line = s.str();
    return line;
}

static inline void strip(std::string &s) {
    size_t index = s.find('#');
    if (index != std::string::npos) {
        s.erase(s.begin() + index, s.end());
    }
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::vector<std::string> split_line(std::string &line) {
    std::vector<std::string> result;
    boost::split(result, line, boost::is_any_of(" "));
    return result;
}

static inline std::vector<std::string> expand_globs(std::vector<std::string> &&args) {
    bool not_used = true;
    std::vector<std::string> res{};
    res.reserve(args.size());
    for (auto &el : args) {
        glob_wrapper::glob_parser glob{el};
        while (glob) {
            res.emplace_back(glob.get_file_name());
            glob.next();
            not_used = false;
        }
        if (not_used) {
            res.emplace_back(std::move(el));
        }
        not_used = true;
    }
    return res;
}

void launch_loop(bool internal_func) {
    std::string line;
    int status = EXIT_SUCCESS;
    std::vector<std::string> tmp;
    do {
        std::vector<std::string> arguments_for_execv;
        line = read_line(internal_func);
        if (line.empty()) {
            return;
        }
        strip(line);
        if (line.empty()) continue;
        expand_vars(line);
        tmp = split_line(line);
        tmp = expand_globs(std::move(tmp));
        arguments_for_execv.reserve(tmp.size() + 1);
        for (auto &parameter : tmp) {
            arguments_for_execv.emplace_back(std::move(parameter));
        }
        status = execute(std::move(arguments_for_execv));
    } while (status == EXIT_SUCCESS);
}

void loop() {
    std::string env = getenv("PATH");
    env += ":" + std::filesystem::current_path().string() + "/bin/";
    setenv("PATH", env.c_str(), 1);
    if (std::filesystem::exists(".myshell_history")) {
        read_history(".myshell_history");
    }
    launch_loop(false);
    matexit();
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

struct parsed_args {
    bool help;
    std::vector<std::string> args;
};

static parsed_args pars(const std::vector<std::string> &argv, const std::string &help_msg) {
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
    } catch (boost::wrapexcept<po::unknown_option> &_e) {}
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << help_msg << "\n" << visible << std::endl;
    }
    return parsed_args{static_cast<bool>(vm.count("help")), vector};
}

int mcd(std::vector<std::string> &argv) {
    const auto parsed = pars(argv, "mcd help");
    if (parsed.help) {
        merrno_val = 0;
        return EXIT_SUCCESS;
    }
    if (argv.empty())
        return no_arguments("mexit");
    if (argv.size() == 1) {
        chdir(getenv("HOME"));
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

int myexec(std::vector<std::string> &argv) {
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
    rl_outstream = stdout;
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

int mexport(std::vector<std::string> &argv) {
    const auto parsed = pars(argv, "mexport help");
    if (parsed.help) {
        merrno_val = 0;
        return EXIT_SUCCESS;
    }
    argv = parsed.args;
    if (argv.empty()) {
        return no_arguments("mexport");
    } else if (argv.size() > 2) {
        return too_many_arguments("mexport");
    }
    constexpr char token{'='};
    std::string expression = argv[1];
    const auto pos = expression.find(token);
    const std::string name = expression.substr(0, pos);
    expression.erase(0, pos + 1);
    global_var_map[name] = expression;
    merrno_val = 0;
    return EXIT_SUCCESS;
}
