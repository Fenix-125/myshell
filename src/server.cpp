// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

/*
  Based on:

  ECHOSERV.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net

  Simple TCP/IP echo server.
*/

#include <server.h>
#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) functions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include <cstring>
#include <iostream>

#include "shell.h"
#include "globals.h"

int out_fd = 0;
__thread char user_ip[24];

void get_addr(std::string &s, struct sockaddr *cl_addr, socklen_t cl_addr_len) {
    auto addr = cl_addr->sa_family;
    if (addr == AF_INET) {
        inet_ntop(AF_INET, &(((struct sockaddr_in *) cl_addr)->sin_addr), user_ip, cl_addr_len);
    } else if (addr == AF_INET6) {
        inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) cl_addr)->sin6_addr), user_ip, cl_addr_len);
    } else {
        strncpy(user_ip, "Unknown AF", cl_addr_len);
    }
    s = user_ip;
}

static bool std_streams_dup(int sock_fd) {
    out_fd = dup(STDOUT_FILENO);
    if (dup2(sock_fd, STDIN_FILENO) == -1) {
        std::cerr << "[myshell] Error while redirecting in/out streams to socket!" << std::endl;
        return false;
    }
    if (dup2(sock_fd, STDOUT_FILENO) == -1) {
        std::cerr << "[myshell] Error while redirecting in/out streams to socket!" << std::endl;
        return false;
    }
    if (dup2(sock_fd, STDERR_FILENO) == -1) {
        std::cerr << "[myshell] Error while redirecting in/out streams to socket!" << std::endl;
        return false;
    }
    return true;
}

void start_server(void(*call_back)(), short int port) {
    int listen_sock;
    int connection_sock;
    struct sockaddr_in serv_addr{};

    std::cout << "starting server" << std::endl;

    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "[myshell] Error: creating listening socket." << std::endl;
        exit(EXIT_FAILURE); // -V2014
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "[myshell] Error calling bind()" << std::endl;
        exit(EXIT_FAILURE); // -V2014
    }

    if (listen(listen_sock, 1000) < 0) {
        std::cout << "[myshell] Error calling listen()\n" << std::endl;
        exit(EXIT_FAILURE); // -V2014
    }

    struct sockaddr cl_addr{};
    socklen_t cl_addr_len;
    std::string addr;
    addr.reserve(20);
    std::cout << "[myshell] Server started" << std::endl;
    while (true) {
        if ((connection_sock = accept(listen_sock, &cl_addr, &cl_addr_len)) < 0) {
            std::cerr << "[myshell] Error calling accept()" << std::endl;
            exit(EXIT_FAILURE); // -V2014
        }

        switch (fork()) {
            case -1:
                std::cerr << "[myshell] Error: failed to do fork!" << std::endl;
                continue;
            case 0: {
                get_addr(addr, &cl_addr, cl_addr_len);
                write_logs("User connected");
                if (std_streams_dup(connection_sock)) {
                    call_back();
                }
                std::cerr << "[myshell] Error: unexpected exit of client!" << std::endl;
                exit(1); // -V2014
            }
            default:
                if (close(connection_sock) < 0) {
                    std::cerr << "[myshell] Error: calling close() in parent for handled connection!" << std::endl;
                    exit(EXIT_FAILURE); // -V2014
                }
        }
    }
}