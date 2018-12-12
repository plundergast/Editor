#ifndef STRING_UTIL_HPP
#define STRING_UTIL_HPP

#include <array>  // For std::array<T,N>
#include <string> // For std::string
#include <vector> // For std::vector<T>

namespace string_util {
    using std::array;
    using std::string;
    using std::vector;

    string to_lower(const string& s);
    string to_upper(const string& s);

    string trim(const string& s);
    string unquote(const string& s);

    array<string,2> split_on_first (const string& str, char delim = ' ');
    vector<string> split_on(const string& line, char c = ' ');

    template <typename pred>
    vector<string> split_if(const string& line, pred&& p);

    array<int, 5> read_face(string str);

    string get_directory(const string& location, bool withSlash = true);

    inline bool is_absolute_path(const string& path) { return path[0] == '/'; }
    inline bool is_relative_path(const string& path) { return path[0] != '/'; }
} // namespace Util

#endif
