// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


//
// Created by myralllka on 9/13/20.
//

#include "file_interface/file_interface_mycp.h"
#include <cerrno>
#include <cstdlib>
#include <locale>
#include <fcntl.h>
#include <filesystem>
#include <iostream>

bool ALL;
int options_processing(std::string const &fname) {
    std::string answer{};
    std::cout << "The file " << fname << " already exists. Do you want to replace it? Y[es]/N[o]/A[ll]/C[ancel] > ";
    if (!std::getline(std::cin, answer)) {
        std::cerr << "Error while reading from command line" << std::endl;
    }
    if (answer == "Y" or answer == "Yes") {
        return 0;
    } else if (answer == "N" or answer == "No") {
        return 1;
    } else if (answer == "A" or answer == "All") {
        return 2;
    } else if (answer == "C" or answer == "Cancel") {
        return 3;
    } else std::cerr << "wrong input" << std::endl;
    return -1;
}

int write_buffer(int fd, char *buffer, ssize_t size) {
    ssize_t written_bytes = 0;
    ssize_t written_now;

    while (written_bytes < size) {
        written_now = write(fd, (void *) (buffer), size - written_bytes);
        if (written_now == -1) {
            if (errno == EINTR)
                continue;
            else {
                return -1;
            }
        } else
            written_bytes += written_now;
    }
    return 0;
}

int processing(int fd_from, int fd_to) {
    ssize_t read_now, read_bytes = 0;
    std::array<char, BUFSIZ> buffer{};
    int res;
    while (true) {
        read_now = read(fd_from, (void *) buffer.data(), BUFSIZ);
        if (read_now == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                return -1;
            }
        } else if (read_now == 0) {
            return static_cast<int>(read_now);
        }
        read_bytes += read_now;
        res = write_buffer(fd_to, buffer.data(), read_now);
        if (res != 0) {
            return res;
        }
        if (read_now < BUFSIZ) break;
    }
    return 0;
}

int copy_file(const std::string &f_from, const std::string &f_to) {
    if (!(ALL) and std::filesystem::exists(f_to)) {
        switch (options_processing(f_to)) {
            case 1:
                // NO
                std::cout << "No: can not copy" << std::endl;
                return 0;
            case 0:
                // YES
                break;
            case 2:
                // ALL
                ALL = true;
                break;
            case 3:
                // CANCEL
                std::cerr << "Canceled" << std::endl;
                return -1;
            case -1:
                break;
            default:
                std::cerr << "unexpected error while options processing" << std::endl;
                return -1;
        }
    }
    auto from = open(f_from.data(), O_RDONLY);
    auto to = open(f_to.data(), O_WRONLY);
    if (to == -1) {
        to = creat64(f_to.data(), S_IROTH | S_IRGRP | S_IWGRP | S_IWUSR | S_IRUSR | S_IRWXU);
    }
    processing(from, to);
    close(from);
    close(to);
    return 0;
}