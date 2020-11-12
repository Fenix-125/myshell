# Shell via remote server

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
./myshell -s -p 6666
```
on the remote
```
telnet IP PORT
```
for example from the other ttyX:
```
telnet localhost 6666
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
- forage ground execution (only if there is space before `&`)
- redirecting of stdin, stdout, stderr
- add subshell variables
- can be used as server

### Additional

- myshell history is in `~/.history` 

## References
- [inlined glob library](https://bitbucket.org/szx/glob/src/master/)