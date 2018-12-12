#pragma once

#include <algorithm> // For std::all_of()
#include <map>       // For std::map<K, V>
#include <vector>    // For std::vector<T>

namespace ini {
	struct section : public std::map<std::string, std::string>
	{
		using super = std::map<std::string, std::string>;

		std::string name;
		bool has (const std::string& name) const { return super::find (name) != super::end (); }
	};

	struct config : public std::map<std::string, section>
	{
		using super = std::map<std::string, section>;

		config () { super::insert ({ "global", section () }); }

		section& get_global_section () { return at ("global"); }
		const section& get_global_section () const { return at ("global"); }

		bool has_section (const std::string& name) const { return super::find (name) != super::end (); }
		bool has_var_in_section (const std::string& var, const std::string& section = "global") const {
			return super::find (section) != super::end ()
				&& super::find (section)->second.has (var);
		}
		bool has_var (const std::string& var) const {
			for (super::const_iterator it = begin (); it != end (); ++it) {
				if (it->second.has (var))
					return true;
			}
			return false;
		}

		void insert (const section& s) { super::insert ({ s.name, s }); }
	};

	bool is_bool (std::string val);
	bool is_int (std::string val);
	bool is_real (std::string val);

	bool   to_bool (std::string val);
	int    to_int (std::string val);
	float  to_float (std::string val);
	double to_double (std::string val);

	std::vector<std::string> to_array (std::string val);

	config parse_file (const std::string& filename);
}