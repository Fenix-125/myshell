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

    [[nodiscard]] bool get_f() const { return f; };

    [[nodiscard]] bool get_R() const { return R; };

    void parse(int ac, char **av);

private:
    std::vector<std::string> filenames;

    bool f = false;
    bool R = false;

    void init_opt_description();

    boost::program_options::variables_map var_map{};
    boost::program_options::options_description opt_conf{
            "Config File Options:\n\tmycp [-h|--help] [-f] <file1> <file2>\nOr:\tmycp [-h|--help] [-f]  <file1> <file2> <file3>... <dir>\nOr:\tmycp [-h|--help] [-f] -R  <dir_or_file_1> <dir_or_file_2> <dir_or_file_3>... <dir>"};
};

#endif //MYCAT_CONFIG_FILE_H

