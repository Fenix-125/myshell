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
#include <utility>
#include <vector>
#include <algorithm>
#include <cctype>
#include <readline/readline.h>
#include <readline/history.h>
#include <boost/program_options.hpp>
#include <fcntl.h>

#include "glob_posix.h"
#include "shell.h"
#include "merrno.h" // extern merrno_val

__thread int merrno_val = 0;

std::unordered_map<std::string, std::string> global_var_map{};

std::map<std::string, std::function<int(std::vector<std::string> &)>> inner_commands = {
        {"mcd",     mcd},
        {"mexit",   mexit},
        {"mpwd",    mpwd},
        {"mecho",   mecho},
        {"mexport", mexport},
        {"merrno",  merrno},
        {".",       myexec}
};

void matexit() {
    write_history(".myshell_history");
    fclose(rl_instream);
    exit(EXIT_FAILURE);
}

int execute(std::vector<std::string> &&argv,
            const pipe_state_t &pipe_state = {false, true, true, false, false, redirections{}, 0},
            [[maybe_unused]] const std::pair<int, std::string> &sub_shell_var = {-2, ""}) {
    pid_t pid = pipe_state.pid;
    int status;
    if (argv.empty()) {
        return EXIT_SUCCESS;
    }

    std::vector<const char *> args_for_execvp;
    args_for_execvp.reserve(argv.size());
    for (const auto &el : argv) {
        args_for_execvp.emplace_back(el.c_str());
    }
    args_for_execvp.emplace_back(nullptr);

//    if (sub_shell_var.first != -2)
//        pid = fork();

    if (pid == 0) {
//        if (sub_shell_var.first != -2) {
//            dup2(sub_shell_var.first, STDOUT_FILENO);
//            close(sub_shell_var.first);
//        }
        if (pipe_state.re) {
            if (pipe_state.red.redirect_in) {
                int input_fd = open(pipe_state.red.fin.c_str(), O_RDONLY);
                if (dup2(input_fd, STDIN_FILENO) == -1) {
                    std::cerr << "Error while redirecting from " << pipe_state.red.fin << std::endl;
                    merrno_val = errno;
                    matexit();
                }
            }
            if (pipe_state.red.redirect_out) {
                int out_fd = open(pipe_state.red.fout.c_str(), O_WRONLY | O_CREAT,
                                  S_IROTH | S_IRGRP | S_IWGRP | S_IWUSR | S_IRUSR | S_IRWXU);
                if (dup2(out_fd, STDOUT_FILENO) == -1) {
                    std::cerr << "Error while redirecting to " << pipe_state.red.fout << std::endl;
                    merrno_val = errno;
                    matexit();
                }
            }
            if (pipe_state.red.redirect_err) {
                int error_fd = open(pipe_state.red.ferr.c_str(), O_WRONLY | O_CREAT,
                                    S_IROTH | S_IRGRP | S_IWGRP | S_IWUSR | S_IRUSR | S_IRWXU);
                if (dup2(error_fd, STDERR_FILENO) == -1) {
                    std::cerr << "Error while redirecting to " << pipe_state.red.ferr << std::endl;
                    merrno_val = errno;
                    matexit();
                }
            }
        } else if (pipe_state.bg) {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
        }
        for (auto &command : inner_commands) {
            if (command.first == argv[0]) {
                status = command.second(argv);
                exit(status);
            }
        }
        if (execvp(args_for_execvp[0], const_cast<char *const *>(args_for_execvp.data()))) {
            if (std::filesystem::path(args_for_execvp[0]).extension() == ".msh") {
                myexec(argv);
            }
            std::cerr << "error while execvp: " << argv[0] << std::endl;
            return EXIT_SUCCESS;
        }
    } else if (pid < 0) {
        // Error forking
        std::cerr << "error while fork" << std::endl;
        merrno_val = ECHILD;
        return EXIT_SUCCESS;
    } else {
        // Parent process
        if (sub_shell_var.first != -2) {
            char buffer[1000];
            ssize_t size = read(sub_shell_var.first, buffer, 1000);
            close(sub_shell_var.first);
            if ((size > 0) && (size < static_cast<ssize_t>(sizeof(buffer)))) {
                buffer[size - 1] = '\0';
                global_var_map[sub_shell_var.second] = buffer;
            }
        }
        if (pipe_state.bg) {
            signal(SIGCHLD, SIG_IGN);
        }
//        else if (sub_shell_var.first != -2) {
//            do {
//                waitpid(pid, &status, WUNTRACED);
//            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
//        }
    }
    merrno_val = 0;
    return EXIT_SUCCESS;
}

std::string read_line(bool internal_func) {
    std::string line;
    line.reserve(1000);
    std::string prompt = std::filesystem::current_path().string() + " $ ";
    if (fileno(rl_instream) == 0) {
        line = readline(prompt.c_str());
        if (line[0] != '\0')
            add_history(line.data());
    } else
        line = readline("");
    if (line.empty()) { // TODO: comments needed here
        if (internal_func)
            matexit();
        else
            return "";
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

bool expand_redirections(std::vector<std::string> &line, redirections &red) {
    if (line.size() <= 2) return false;
    if (line[line.size() - 2] == ">") {
        red.redirect_out = true;
        red.fout = line.back();
    } else if (line.size() <= 4 and line[line.size() - 3] == ">" and line.back() == "2>&1") {
        red.redirect_out = true;
        red.redirect_err = true;
        red.fout = line[line.size() - 2];
        red.ferr = line[line.size() - 2];
        line.pop_back();
    } else if (line[line.size() - 2] == "2>") {
        red.redirect_err = true;
        red.ferr = line.back();
    } else if (line[line.size() - 2] == "<") {
        red.redirect_in = true;
        red.fin = line.back();
    } else if (line[line.size() - 2] == "&>") {
        red.redirect_out = true;
        red.redirect_err = true;
        red.ferr = line.back();
        red.fout = line.back();
    } else {
        return false;
    }
    line.pop_back();
    line.pop_back();
    return true;
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

struct pipe_desc_t {
    int in, out;
};

struct pipe_proc_t {
    pipe_proc_t(std::vector<std::string> &&command, pipe_desc_t &&pipe, pipe_state_t &&pipe_state)
            : command(std::move(command)), pipe(pipe), pipe_state(std::move(pipe_state)) {}

    std::vector<std::string> command;
    pipe_desc_t pipe;
    pipe_state_t pipe_state;
};

static inline int close_all_pipes(const std::vector<pipe_proc_t> &pipeline) {
    int exit_status = EXIT_SUCCESS;
    for (const auto &pipe_el : pipeline) {
        if (pipe_el.pipe.in > 2)
            if (close(pipe_el.pipe.in) == -1)
                exit_status = EXIT_FAILURE;
        if (pipe_el.pipe.out > 2)
            if (close(pipe_el.pipe.out) == -1)
                exit_status = EXIT_FAILURE;
    }
    return exit_status;
}

static std::vector<pipe_proc_t> build_pipeline(std::string &&line) {
    size_t pipeline_size = std::count(line.begin(), line.end(), '|') + 1;
    int tmp_desc[2];
    std::vector<pipe_proc_t> pipeline{};
    std::vector<std::string> arguments_for_execv;
    pipeline.reserve(pipeline_size);
    bool bg = false, re;
    redirections rd;

//    strip(line); // WARNING: at this moment the line should be striped
    if (line[line.size() - 1] == '&') {
        line.pop_back();
        bg = true;
    }
    line = expand_vars(line);
    if (pipeline_size == 1) {
        arguments_for_execv = split_line(line);
        re = expand_redirections(arguments_for_execv, rd);
        arguments_for_execv = expand_globs(std::move(arguments_for_execv));

        pipeline.emplace_back(std::move(arguments_for_execv), pipe_desc_t{STDIN_FILENO, STDOUT_FILENO},
                              pipe_state_t{false, true, true, bg, re, rd, 0});
        return pipeline;
    }

    std::vector<std::string> commands{};
    commands.reserve(pipeline_size);
    boost::split(commands, std::move(line), boost::is_any_of("|"));
    for (auto &command : commands)
        strip(command);
    arguments_for_execv = split_line(commands[0]);
    re = expand_redirections(arguments_for_execv, rd);
    arguments_for_execv = expand_globs(std::move(arguments_for_execv));
    pipeline.emplace_back(std::move(arguments_for_execv), pipe_desc_t{STDIN_FILENO, STDOUT_FILENO},
                          pipe_state_t{true, true, false, bg, re, rd, 0});
    for (size_t i = 1; i < pipeline_size; ++i) {
        if (pipe(tmp_desc) == -1) {
            std::cerr << "Error: while creating pipe!" << std::endl;
            merrno_val = EPIPE;
            close_all_pipes(pipeline);
            return std::vector<pipe_proc_t>{};
        }
        pipeline[i - 1].pipe.out = tmp_desc[1];
        arguments_for_execv = split_line(commands[i]);
        re = expand_redirections(arguments_for_execv, rd);
        arguments_for_execv = expand_globs(std::move(arguments_for_execv));
        pipeline.emplace_back(std::move(arguments_for_execv),
                              pipe_desc_t{tmp_desc[0], -1},
                              pipe_state_t{true, false, false, bg, re, rd, 0});
    }
    pipeline[pipeline_size - 1].pipe.out = STDOUT_FILENO;
    pipeline[pipeline_size - 1].pipe_state.last_pipe = true;
    return pipeline;
}

static inline void kill_pipeline(const std::vector<pipe_proc_t> &pipeline) {
    for (auto &pipe_el : pipeline) {
        if (kill(pipe_el.pipe_state.pid, SIGTERM) == -1) {
            std::cerr << "Error: fail to kill '" << pipe_el.command[0] << "' (pid=" << pipe_el.pipe_state.pid << ")"
                      << std::endl;
        } // TODO: check if is valid to kill here
    }
}

static int run_pipeline(std::vector<pipe_proc_t> &&pipeline, const std::pair<bool, std::string> &subs = {}) {
    if (pipeline.empty()) {
        return EXIT_SUCCESS;
    }
    bool in_parent;
    int status;
    pipe_desc_t subs_pipe_fd{};
    if (subs.first) {
        int sub_pipe[2];
        if (pipe(sub_pipe) == -1) {
            std::cerr << "Error: could not create subshell pipe!" << std::endl;
        }
        subs_pipe_fd.in = sub_pipe[0];
        subs_pipe_fd.out = sub_pipe[1];
    }

    for (auto &pipe_el : pipeline) {
        if (pipe_el.command.empty())
            return EXIT_SUCCESS;
        if ((!pipe_el.pipe_state.first_pipe && pipe_el.pipe_state.red.redirect_in) ||
            (!pipe_el.pipe_state.last_pipe && pipe_el.pipe_state.red.redirect_out) ||
            (pipe_el.pipe_state.last_pipe && pipe_el.pipe_state.red.redirect_out && subs.first)) {
            std::cerr << "Error: bad redirect in pipeline" << std::endl;
            return EXIT_SUCCESS;
        }
        switch (pipe_el.pipe_state.pid = fork()) {
            case -1:
                std::cerr << "Error: fail to fork" << std::endl;
                return EXIT_SUCCESS;
            case 0:             /* First child: exec 'ls' to write to pipe */
                in_parent = false;
                break;
            default:            /* Parent falls through to create next child */
                in_parent = true;
                break;
        }
        if (!in_parent) {
            if (pipe_el.pipe.in != STDIN_FILENO)
                if (dup2(pipe_el.pipe.in, STDIN_FILENO) == -1)
                    std::cerr << "dup2 in" << std::endl;
            if (pipe_el.pipe.out != STDOUT_FILENO)
                if (dup2(pipe_el.pipe.out, STDOUT_FILENO) == -1)
                    std::cerr << "dup2 out" << std::endl;
            if (close_all_pipes(pipeline) == EXIT_FAILURE) {
                std::cerr << "Error: while closing pipes in child!" << std::endl;
            }
            if (subs.first && pipe_el.pipe_state.last_pipe) {
                if (dup2(subs_pipe_fd.out, STDOUT_FILENO) == -1) {
                    std::cerr << "dup2 sub" << std::endl;
                    return EXIT_SUCCESS;
                }
                if (close(subs_pipe_fd.in) == -1 || close(subs_pipe_fd.out) == -1) {
                    std::cerr << "close sub in child" << std::endl;
                    return EXIT_SUCCESS;
                }
            }
            status = execute(std::move(pipe_el.command), pipe_el.pipe_state);
            std::cerr << "Error: fail to execute in child" << std::endl;
            return EXIT_FAILURE;
        } else if (subs.first && pipe_el.pipe_state.last_pipe) {
            if (close(subs_pipe_fd.out) == -1) {
                std::cerr << "close sub in child" << std::endl;
                return EXIT_SUCCESS;
            }
            if (close_all_pipes(pipeline) == EXIT_FAILURE) {
                std::cerr << "Error: while closing pipes in parent" << std::endl;
                kill_pipeline(pipeline); // TODO: review
                return EXIT_SUCCESS;
            }
            status = execute(std::move(pipe_el.command), pipe_el.pipe_state,
                             std::pair<int, std::string>{subs_pipe_fd.in, subs.second});
        } else {
            status = execute(std::move(pipe_el.command), pipe_el.pipe_state);
        }
        if (status != EXIT_SUCCESS) {
            kill_pipeline(pipeline);
            return EXIT_SUCCESS;
        }
    }
    /* Parent closes unused file descriptors for pipe, and waits for children */
    if (!subs.first)
        if (close_all_pipes(pipeline) == EXIT_FAILURE) {
            std::cerr << "Error: while closing pipes in parent" << std::endl;
            kill_pipeline(pipeline); // TODO: review is it needed
            return EXIT_SUCCESS;
        }
    for (auto &pipe_el : pipeline) {
        do {
            waitpid(pipe_el.pipe_state.pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        if (status == -1) {
            std::cerr << "Error: fail to wait pipeline element with status: " << status << std::endl;
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}

bool expand_subshell(const std::string &line) {
    auto dindex = line.find('$');
    auto lindex = line.find('(');
    auto rindex = line.rfind(')');
    int status = EXIT_SUCCESS;
    if (lindex != std::string::npos and
        rindex != std::string::npos and
        dindex != std::string::npos and
        dindex + 1 == lindex) {
        std::string name = line.substr(0, lindex - 2);
        auto substr = line.substr(lindex + 1, rindex - lindex - 1);
        if (substr.empty())
            return EXIT_SUCCESS;
        strip(substr);
        if (substr.empty())
            return EXIT_SUCCESS;
        status = run_pipeline(build_pipeline(std::move(substr)), std::pair<bool, std::string>{true, name});
        if (status != EXIT_SUCCESS) {
            std::cerr << "myshell: Error while executing subshell." << std::endl;
            matexit();
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE; // continue to parsing pipeline
}

void launch_loop(bool internal_func) {
    std::string line;
    int status = EXIT_SUCCESS;
    do {
        line = read_line(internal_func);
        if (line.empty()) {
            continue;
        }
        strip(line);
        if (line.empty())
            continue;
        status = expand_subshell(line);
        if (status == EXIT_SUCCESS) {
            continue;
        }
        status = run_pipeline(build_pipeline(std::move(line)));
    } while (status == EXIT_SUCCESS);
}

void loop() {
    auto e = getenv("PATH");
    if (e == nullptr)
        std::cerr << "Error getting PATH!" << std::endl;
    else {
        auto env = std::string{e};
        env += ":" + std::filesystem::current_path().string() + "/bin/";
        setenv("PATH", env.c_str(), 1);
        if (std::filesystem::exists(".myshell_history")) {
            read_history(".myshell_history");
        }
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
    try {
        po::notify(vm);
    } catch (const boost::exception &e) {
        std::cerr << "Error: while parsing parameters";
        exit(1);
    }
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
        auto tmp = getenv("HOME");
        if (tmp == nullptr)
            std::cerr << "Error reading HOME variable!" << std::endl;
        else
            chdir(tmp);
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
