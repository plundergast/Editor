#ifndef STRING_UTIL_HPP
#define STRING_UTIL_HPP

#include <array>  // For std::array<T,N>
#include <string> // For std::string
#include <vector> // For std::vector<T>

namespace Util {
    using std::array;
    using std::string;
    using std::vector;

    string ToLower(const string& s);
    string ToUpper(const string& s);

    string Trim(const string& s);
    string Unquote(const string& s);

    vector<string> SplitOnFirst(const string& line, char c = ' ');
    vector<string> SplitOn(const string& line, char c = ' ');

    template <typename Pred>
    vector<string> SplitIf(const string& line, Pred&& p);

    array<int, 5> ReadFace(string str);

    string GetDirectory(const string& location, bool withSlash = true);

    inline bool IsAbsolutePath(const string& path) { return path[0] == '/'; }
    inline bool IsRelativePath(const string& path) { return path[0] != '/'; }
} // namespace Util

#endif
