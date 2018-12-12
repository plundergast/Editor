#include "StringUtil.hpp"

#include <sstream> // For std::stringstream
#include <algorithm> // For std::transform()
#include <cctype>  // For ::toupper()

namespace Util {
    string ToLower(const string& s) {
	string res(s);
	std::transform(res.begin(), res.end(), res.begin(), ::tolower);
	return res;
    }

    string ToUpper(const string& s) {
	string res(s);
	std::transform(res.begin(), res.end(), res.begin(), ::toupper);
	return res;
    }

    vector<string> SplitOnFirst(const string& str, char delim){
	vector<string> res;
	res.reserve(2);

	if(str.find_first_of(delim) == std::string::npos){ 
	    res.push_back(str);
	    return res;
	}

	res.push_back(str.substr(0, str.find_first_of(delim)));
	res.push_back(str.substr(str.find_first_of(delim)));

	return res;
    }

    string Trim(const string& s) {
	string res(s);

	if(isspace(res[0])) {
	    size_t i;
	    for(i = 0; i < res.length() && isspace(res[i]); i++); // Trim front
	    res.erase(0, i);
	}

	if(isspace(res.back())) {
	    int ii;
	    for(ii = res.length() - 1; ii >= 0 && isspace(res[ii]); ii--); // Trim back
	    if(isspace(res[ii+1]))
		res.erase(ii+1);
	}

	return res;
    }

    vector<string> SplitOn(const string& str, char delim) {
	vector<string> res;
	res.reserve(str.length() / 4);

	std::stringstream ss;
	int prevI = 0;

	for(unsigned i = 0; prevI + i <= str.length(); i++) {
	    char c = str[i + prevI];
	    if(c == delim || (prevI + i == str.length() && ss.str().length())) {
		res.push_back(ss.str());
		ss.str("");
		prevI += i;
		i = 0;
	    } else {
		ss << c;
	    }
	}

	return res;
    }

    template <typename Pred>
    vector<string> SplitIf(const string& str, Pred&& p) {
	vector<string> res;

	size_t prevI = 0;
	for(size_t i = 0; prevI + i < str.length(); i++) {
	    if(p(str[prevI + i])) {
		res.push_back(str.substr(prevI, i));
		prevI += i;
		i = 0;
	    }
	}

	res.push_back(str.substr(prevI));

	return res;
    }

    array<int, 5> ReadFace(string str) {
	array<int, 5> res{ { 0, 0, 0, 0, 0 } };

	str    = Trim(str);
	auto v = SplitOn(str, '/');

	res[0] = stoi(v[0]);

	if(v.size() > 1)
	    res[1] = v[1].size();
	if(res[1])
	    res[2] = stoi(v[1]);

	if(v.size() > 2)
	    res[3] = v[2].size();
	if(res[3])
	    res[4] = stoi(v[2]);

	return res;
    }

    string Unquote(const string& s) {
	string res = Trim(s);

	if(res.front() == '\"')
	    res.erase(0, 1);

	if(res.back() == '\"')
	    res.pop_back();

	return res;
    }

    string GetDirectory(const string& location, bool withSlash) {
	auto i = location.find_last_of('/');

	if(i == string::npos) {
	    return (withSlash ? "./" : ".");
	} else {
	    return location.substr(0, i + (withSlash ? 1 : 0));
	}
    }
} // namespace Util
