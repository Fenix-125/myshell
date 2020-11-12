// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//
// Created by myralllka on 10/31/20.
//

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "glob_posix.h"
#include "line_operations.hpp"
#include "commands.hpp"
#include "globals.h"

std::vector<std::string> expand_globs(std::vector<std::string> &&args) {
    bool not_used = true;
    std::vector<std::string> res{};
    res.reserve(args.size());
    for (auto &el : args) {
        glob_wrapper::glob_parser glob{el};
        while (glob) {
            res.emplace_back(glob.get_file_name());
            glob.next();
            not_used = false;
        }
        if (not_used) {
            res.emplace_back(std::move(el));
        }
        not_used = true;
    }
    return res;
}

std::string read_line(bool internal_func) {
    std::string line;
    line.reserve(1000);
    std::string prompt = std::filesystem::current_path().string() + " $ ";
    line = readline(prompt.c_str());
    if (state == 2) {
        write_logs(line);
    }
    if (line[0] != '\0')
        add_history(line.data());
    if (line.empty()) {
        if (internal_func)
            matexit();
        else
            return "";
    }
    return line;
}

void strip(std::string &s) {
    size_t index = s.find('#');
    if (index != std::string::npos) {
        s.erase(s.begin() + index, s.end());
    }
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


std::vector<std::string> split_line(std::string &line) {
    std::vector<std::string> result;
    boost::split(result, line, boost::is_any_of(" "));
    return result;
}

bool expand_redirections(std::vector<std::string> &line, redirections &red) {
    if (line.size() <= 2) return false;
    if (line[line.size() - 2] == ">") {
        red.redirect_out = true;
        red.fout = line.back();
    } else if (line.size() <= 4 and line[line.size() - 3] == ">" and line.back() == "2>&1") { // -V112
        red.redirect_out = true;
        red.redirect_err = true;
        red.fout = line[line.size() - 2];
        red.ferr = red.fout;
        line.pop_back();
    } else if (line[line.size() - 2] == "2>") {
        red.redirect_err = true;
        red.ferr = line.back();
    } else if (line[line.size() - 2] == "<") {
        red.redirect_in = true;
        red.fin = line.back();
    } else if (line[line.size() - 2] == "&>") {
        red.redirect_out = true;
        red.redirect_err = true;
        red.ferr = line.back();
        red.fout = red.ferr;
    } else {
        return false;
    }
    line.pop_back();
    line.pop_back();
    return true;
}