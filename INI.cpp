#include "INI.hpp"

#include <fstream> // For std::ifstream
#include "StringUtil.hpp" // For SWAN::Util::Trim(), SWAN::Util::ToLower()

using std::string;

namespace INI {
    bool IsBool(std::string val) {
	val = Util::ToLower(Util::Trim(val));
	return val == "true" || val == "false" || val == "on" || val == "off" || IsInt(val);
    }
    bool IsInt (std::string val) {
	val = Util::Trim(val);
	return std::all_of(val.begin(), val.end(), [](char c) {return std::isdigit(c);});
    }
    bool IsReal(std::string val) {
	val = Util::Trim(val);
	return std::all_of(val.begin(), val.end(),
			   [dotFound = false](char c) mutable -> bool {
			       if(c == '.' || c == ',') {
				   if(!dotFound) {
				       dotFound = true;
				       return true;
				   } else {
				       return false;
				   }
			       }
			       
			       return std::isdigit(c);
			   });
    }


    bool ToBool(std::string val) {
	val = Util::Trim(Util::ToLower(val));
	if(IsInt(val))
	    return ToInt(val);
	return val == "true" || val == "on";
    }

    int ToInt(std::string val) {return std::stoi(Util::Trim(val));}
    float ToFloat(std::string val) {return std::stof(Util::Trim(val));}
    double ToDouble(std::string val) {return std::stod(Util::Trim(val));}

    std::vector<std::string> ToArray(std::string val) {
	auto res = Util::SplitOn(val, ',');
	std::transform(res.begin(), res.end(), res.begin(), Util::Trim);
	res.resize(std::distance (res.begin(),
				  std::remove_if(res.begin(), res.end(),
						 [](const std::string& s){ return s.length() == 0; })));
	return res;
    }

    Config ParseFile(const std::string& filename) {
	Config res;
	Section* currSec = &res["global"];

	std::ifstream file(filename);

	//size_t lineNum = 0;
	std::string line;

	while(std::getline(file, line)) {
	    //lineNum++;
	    line = Util::Trim(line);

	    if(line[0] == '#' || line.length() == 0)
		continue;
	    else if(line.length() < 3)
		continue;
	    else if(line[0] != '[' && line.find('=') == std::string::npos)
		continue;
	    
	    // Extract text without comments at back of line

	    // FIXIT: This is stupid, because a # should be able to appear in strings.
	    //        Should instead first figure out type of line,
	    //        then separate and ONLY THEN kill off comments at EOL.
	    // Current solution: Just disallow comments on same line as non-comments
	    //line = Util::Trim(Util::SplitOnFirst(line, '#')[0]);

	    if(line[0] == '[') {
		string section = line.substr(1, line.find_first_of(']')-1);
		if(!res.hasSection(section))
		    res[section].name = section;

		currSec = &res[section];
	    } else {
		auto v = Util::SplitOn(line, '=');
		if(v.size() > 2)
		    continue;
		else if(v.size() < 2)
		    continue;

		string name  = Util::Unquote(Util::Trim(v[0]));
		string value = Util::Unquote(Util::Trim(v[1]));

		if(name.length() == 0)
		    continue;
		else if(value.length() == 0)
		    continue;

		currSec->insert({name, value});
	    }
	}

	return res;
    }
}
