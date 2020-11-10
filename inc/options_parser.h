#ifndef MYCAT_CONFIG_FILE_H
#define MYCAT_CONFIG_FILE_H

#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <fstream>

class command_line_options {
public:
    command_line_options();

    ~command_line_options() = default;

    [[nodiscard]] std::vector<std::string> get_filenames() const { return filenames; };

    [[nodiscard]] short int get_port() const { return port; };

    [[nodiscard]] bool get_server() const { return server; };

    static std::string assert_file_exist(const std::string &f_name);

    void parse(int ac, char **av);

private:
    std::vector<std::string> filenames;
    short int port = 7777;
    bool server = false;

    void init_opt_description();

    boost::program_options::variables_map var_map{};
    boost::program_options::options_description opt_conf{
            "Config File Options:\n\tmyshell [-h|--help] <filename.msh>\n\tfilename.msh - a script to be executed using myshell. *.msh extension required. Only one file available"};
};

#endif //MYCAT_CONFIG_FILE_H

