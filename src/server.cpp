/*
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

/*  Global constants  */
#define LISTEN_Q_SIZE      (1024)   //  Backlog for listen()
//#define MAX_LINE           (1000)


static inline char *get_addr(char *s, struct sockaddr *cl_addr, socklen_t cl_addr_len) {
    switch (cl_addr->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *) cl_addr)->sin_addr), s, cl_addr_len);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) cl_addr)->sin6_addr), s, cl_addr_len);
            break;

        default:
            strncpy(s, "Unknown AF", cl_addr_len);
            return nullptr;
    }
    return s;
}

static bool std_streams_dup(int sock_fd) {
    if (dup2(sock_fd, STDIN_FILENO) == -1) {
        std::cerr << "[myshell] Error while redirecting in/out streams to socket!" << std::endl;
        return false;
    }
    if (dup2(sock_fd, STDOUT_FILENO) == -1) {
        std::cerr << "[myshell] Error while redirecting in/out streams to socket!" << std::endl;
        return false;
    }
//    if (dup2(sock_fd, STDERR_FILENO) == -1) {
//        std::cerr << "[myshell] Error while redirecting in/out streams to socket!" << std::endl;
//        return false;
//    }
    return true;
}

void start_server(void(*call_back)(), short int port) {
    int listen_sock;
    int connection_sock;
    struct sockaddr_in serv_addr{};

    std::cout << "starting server" << std::endl;

    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "[myshell] Error: creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "[myshell] Error calling bind()\n");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_sock, LISTEN_Q_SIZE) < 0) {
        fprintf(stderr, "[myshell] Error calling listen()\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr cl_addr{};
    socklen_t cl_addr_len;
    std::cout << "[myshell] Server started" << std::endl;
    while (true) {
        if ((connection_sock = accept(listen_sock, &cl_addr, &cl_addr_len)) < 0) {
            fprintf(stderr, "[myshell] Error calling accept()\n");
            exit(EXIT_FAILURE);
        }

        switch (fork()) {
            case -1:
                std::cerr << "[myshell] Error: failed to do fork!" << std::endl;
                continue;
            case 0:
//                handle_client(connection_sock, &cl_addr, cl_addr_len);
                if (std_streams_dup(connection_sock)) {
                    call_back();
                }
                std::cerr << "[myshell] Error: unexpected exit of client!" << std::endl;
                exit(1);
            default:
                if (close(connection_sock) < 0) {
                    fprintf(stderr, "[myshell] Error: calling close() in parent for handled connection!\n");
                    exit(EXIT_FAILURE);
                }
        }
    }
}

/*
void handle_client(int conn_s, struct sockaddr *cl_addr, socklen_t cl_addr_len) {
    char addr[20];
    get_addr(addr, cl_addr, cl_addr_len);
    char buffer[MAX_LINE];
    ssize_t status;

    std::cout << "[myshell] Note: New client(" << addr << ") connected" << std::endl;
    while ((status = read_line(conn_s, buffer, MAX_LINE - 1)) > 0) {
        if (write_line(conn_s, buffer, strlen(buffer)) == -1) {
            std::cerr << "[myshell] Error: could not write back to client(" << addr << ")!" << std::endl;
            close(conn_s);
            std::cout << "[myshell] Note: Client(" << addr << ") closed connection" << std::endl;
            exit(1);
        }
    }
    close(conn_s);
    if (status == -1) {
        std::cerr << "[myshell] Error: could not read from client(" << addr << ")!" << std::endl;
        std::cout << "[myshell] Note: Client(" << addr << ") closed connection" << std::endl;
        exit(1);
    }
    std::cout << "[myshell] Note: Client(" << addr << ") closed connection" << std::endl;
    exit(0);
}
*/
