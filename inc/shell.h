//
// Created by myralllka on 9/22/20.
//

#ifndef LAB3_MYSHELL_SHELL_H
#define LAB3_MYSHELL_SHELL_H


#include <map>

class shell {
public:
    shell();

    ~shell();

//    int execute();
    int execute(const std::string &program_name, char **argv);

    int loop();

    int update_history();

private:
    std::string pwd;
    int error_code = 0;
    std::map<std::string, std::string> local_variables;
};


#endif //LAB3_MYSHELL_SHELL_H
