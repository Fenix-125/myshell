# Lab 3 fork-exec: myshell

## Team

 - [Yuriy Pasichnyk](https://github.com/Fenix-125)
 - [Morhunenko Mykola](https://github.com/Myralllka)

## Prerequisites

 - **C++ compiler** - needs to support **C++17** standard
 - **CMake** 3.15+
 
The rest prerequisites (such as development libraries) can be found in the [packages file](./apt_packages.txt) in the form of the apt package manager package names.

## Installing
```
git clone https://github.com/Fenix-125/myshell.git
cd myshell
bash compile.sh
```
## Usage
```
./myshell
```
## Internal commands:
- mcd
- mpwd
- mexit
- merrno
- mecho
- mexport
## Outer commands:
- mycp
- mycat
## Supported features:
### Obligatory
- interpreter prompt shows current path
- internal (list above) and external commands (git, ls, whoami etc)
- ./bin is added to the PATH, so any program from that folder can be launched
- GLOB\`s supporting
- commands history (between launches) and command line editing
- comments
- scripts supporting (but without forks and loops)

### Additional

-

## References
- [inlined glob library](https://bitbucket.org/szx/glob/src/master/)