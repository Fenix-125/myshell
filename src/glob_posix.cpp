// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 *
 * (c)
 *
 * */

#include <fnmatch.h>
#include "glob_posix.h"

static std::pair<std::string, std::string> split_path(const std::string &path) {
    std::string::size_type last_sep = path.find_last_of('/');
    if (last_sep != std::string::npos) {
        return std::make_pair(std::string(path.begin(), path.begin() + last_sep),
                              std::string(path.begin() + last_sep + 1, path.end()));
    }
    return std::make_pair(".", path);
}

namespace glob_wrapper {

    glob_parser::glob_parser(const std::string &pattern) : dir_(nullptr), dir_entry_(nullptr) {
        std::pair<std::string, std::string> dir_and_mask = split_path(pattern);

        dir_ = opendir(dir_and_mask.first.c_str());
        pattern_ = dir_and_mask.second;

        if (dir_ != nullptr) {
            next();
        }
    }

    glob_parser::~glob_parser() {
        if (dir_ != nullptr) {
            closedir(dir_);
        }
    }

    bool glob_parser::next() {
        while ((dir_entry_ = readdir(dir_)) != nullptr) {
            if (!fnmatch(pattern_.c_str(), dir_entry_->d_name, FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD)) {
                return true;
            }
        }
        return false;
    }

    std::string glob_parser::get_file_name() const {
        assert(dir_entry_ != nullptr);
        return dir_entry_->d_name;
    }

} // namespace glob_wrapper
