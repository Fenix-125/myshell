// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <unistd.h>
#include <sys/types.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
extern char **environ;


int main(int argc, char* argv[]) {
    pid_t parent = getppid();
    pid_t pid = getpid();
    std::cout << "Child: Hello from child!" << std::endl;
    std::cout << "Child: Parent PID: " << parent << ", child PID: " << pid << endl;
    auto path_ptr = getenv("PATH");
    if(path_ptr)
        std::cout << "Child: My PATH variable: " << path_ptr << std::endl;
    else
        std::cout << "Child: PATH variable absent." << std::endl;

    cout << endl;
    for(int i = 0; i<argc; ++i)
    {
        std::cout << "Child: Arg No " << i << " |" << argv[i] << "|" << std::endl;
    }
    return 0;
}