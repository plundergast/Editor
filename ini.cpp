#include "ini.hpp"

#include <fstream> // For std::ifstream
#include "string_util.hpp" // For SWAN::Util::Trim(), SWAN::Util::ToLower()
#include <cctype>

using std::string;

namespace ini
{
	bool is_bool (std::string val)
	{
		val = string_util::to_lower (string_util::trim (val));
		return val == "true" || val == "false" || val == "on" || val == "off" || is_int (val);
	}
	bool is_int (std::string val)
	{
		val = string_util::trim (val);
		return std::all_of (val.begin (), val.end (), [](char c) { return std::isdigit (c); });
	}
	bool is_real (std::string val)
	{
		val = string_util::trim (val);
		return std::all_of (val.begin (), val.end (),
			[dotFound = false] (char c) mutable -> bool {
			if (c == '.' || c == ',')
			{
				if (!dotFound)
				{
					dotFound = true;
					return true;
				}
				else
				{
					return false;
				}
			}

			return std::isdigit (c);
		});
	}


	bool to_bool (std::string val)
	{
		val = string_util::trim (string_util::to_lower (val));
		if (is_int (val))
			return to_int (val);
		return val == "true" || val == "on";
	}

	int to_int (std::string val) { return std::stoi (string_util::trim (val)); }
	float to_float (std::string val) { return std::stof (string_util::trim (val)); }
	double to_double (std::string val) { return std::stod (string_util::trim (val)); }

	std::vector<std::string> to_array (std::string val)
	{
		auto res = string_util::split_on (val, ',');
		std::transform (res.begin (), res.end (), res.begin (), string_util::trim);
		res.resize (std::distance (res.begin (),
			std::remove_if (res.begin (), res.end (),
				[](const std::string& s) { return s.length () == 0; })));
		return res;
	}

	config parse_file (const std::string& filename)
	{
		config res;
		section* currSec = &res["global"];

		std::ifstream file (filename);

		//size_t lineNum = 0;
		std::string line;

		while (std::getline (file, line))
		{
			//lineNum++;
			line = string_util::trim (line);

			if (line[0] == '#' || line.length () == 0)
				continue;
			else if (line.length () < 3)
				continue;
			else if (line[0] != '[' && line.find ('=') == std::string::npos)
				continue;

			// Extract text without comments at back of line

			// FIXIT: This is stupid, because a # should be able to appear in strings.
			//        Should instead first figure out type of line,
			//        then separate and ONLY THEN kill off comments at EOL.
			// Current solution: Just disallow comments on same line as non-comments
			//line = Util::Trim(Util::SplitOnFirst(line, '#')[0]);

			if (line[0] == '[')
			{
				string section = line.substr (1, line.find_first_of (']') - 1);
				if (!res.has_section (section))
					res[section].name = section;

				currSec = &res[section];
			}
			else
			{
				auto v = string_util::split_on (line, '=');
				if (v.size () > 2)
					continue;
				else if (v.size () < 2)
					continue;

				string name = string_util::unquote (string_util::trim (v[0]));
				string value = string_util::unquote (string_util::trim (v[1]));

				if (name.length () == 0)
					continue;
				else if (value.length () == 0)
					continue;

				currSec->insert ({ name, value });
			}
		}

		return res;
	}
}
