#include <cassert>
#include <string>
#include <dirent.h>

namespace glob_wrapper {

    class glob_parser {
    public:
        explicit glob_parser(const std::string &pattern);

        ~glob_parser();

        [[nodiscard]] std::string get_file_name() const;

        explicit operator bool() const {
            return dir_entry_ != 0;
        }

        bool next();

    private:
        glob_parser(const glob_parser &);

        void operator=(const glob_parser &);

    private:
        std::string pattern_;
        DIR *dir_; // -V122
        struct dirent *dir_entry_; // -V122
    };

} // namespace glob_wrapper
