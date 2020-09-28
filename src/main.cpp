// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//#include <iostream>
//#include <string>
//#include <vector>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/wait.h>
////#include <boost/program_options.hpp>
//#include <unordered_set>
//#include <unordered_map>

#include <iostream>
#include "shell.h"

int main()
{
    // Load config files, if any.

    // Run command loop.
    loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}

//int main(int argc, char **argv) {
//    /*
//    int variable_a, variable_b;
//
//    namespace po = boost::program_options;
//
//    po::options_description visible("Supported options");
//    visible.add_options()
//            ("help,h", "Print this help message.");
//
//    po::options_description hidden("Hidden options");
//    hidden.add_options()
//            ("a", po::value<int>(&variable_a)->default_value(0), "Variable A.")
//            ("b", po::value<int>(&variable_b)->default_value(0), "Variable B.");
//
//    po::positional_options_description p;
//    p.add("a", 1);
//    p.add("b", 1);
//
//    po::options_description all("All options");
//    all.add(visible).add(hidden);
//
//    po::variables_map vm;
//    po::store(po::command_line_parser(argc, argv).options(all).positional(p).run(), vm);
//    po::notify(vm);
//
//    if (vm.count("help")) {
//        std::std::cout << "Usage:\n  add [a] [b]\n" << visible << std::std::endl;
//        return EXIT_SUCCESS;
//    }
//
//    int result = operations::add(variable_a, variable_b);
//    std::std::cout << result << std::std::endl;
//    */
//
//    std::string command_buf{};
//    std::unordered_set<std::string> commands = {"ls", "pwd", "echo"};
////    std::unordered_map<std::string, std::string> commands = {
////            {"ls", "ls"},
//////            {"cd", ""},
////            {"pwd", "pwd"},
////            {"echo", "echo"}
////    };
//
//    while (true) {
//        std::cout << ">> " << std::flush;
//        std::getline(std::cin, command_buf);
//        if (command_buf == "exit")
//            exit(EXIT_SUCCESS);
//        else if (commands.find(command_buf) == commands.end())
//            continue;
//        pid_t parent = getpid();
//        pid_t pid = fork();
//
//        if (pid == -1) {
//            std::cerr << "Failed to fork()" << std::endl;
//            exit(EXIT_FAILURE);
//        } else if (pid > 0) {
//            // We are parent process
//            int status;
//            waitpid(pid, &status, 0);
//            std::cout << "child exit code: " << status << std::endl;
//        } else {
//            std::vector<std::string> args;
//            args.emplace_back(command_buf); // Zero argument should be program name
////        args.emplace_back("arg1")
///*
////        auto path_ptr = getenv("PATH");
////        std::string path_var;
////        if (path_ptr != nullptr)
////            path_var = path_ptr;
////        path_var += ":.";
////        setenv("PATH", path_var.c_str(), 1);
//            //! Environment is ready
//            */
//            //! Prepare args array in form suitable for execvp
//            std::vector<const char *> arg_for_c;
//            arg_for_c.reserve(args.size());
//            for (const auto &s: args)
//                arg_for_c.emplace_back(s.c_str());
//            arg_for_c.push_back(nullptr);
//
//            //! const_cast використано, так як не бачу іншого способу
//            //! з const char** отримати char* const*, який хоче ф-ція
//            execvp(command_buf.c_str(), const_cast<char *const *>(arg_for_c.data()));
//
//            std::cerr << "Parent: Failed to execute " << command_buf << " \n\tCode: " << errno << std::endl;
//            exit(EXIT_FAILURE);   // exec never returns
//        }
//    }
//
//    return EXIT_SUCCESS;
//}
