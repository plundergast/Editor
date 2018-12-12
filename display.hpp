#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <memory>
#include "string_util.hpp"
#include "ini.hpp"

extern const std::map<std::string, olc::Pixel> predefined_colors;

static inline const int hex_char_to_int (char hex)
{
	if (hex >= '0' && hex <= '9')
		return hex - '0';
	else
	{
		if (hex >= 'A' && hex <= 'Z')
		{
			return hex - 'A' + 10;
		}
		else if (hex >= 'a' && hex <= 'z')
		{
			return hex - 'a' + 10;
		}
	}

	return -1;
}

static inline olc::Pixel parse_color (std::string colorStr)
{
	colorStr = string_util::to_lower (string_util::trim (colorStr));
	auto iter = predefined_colors.find (colorStr);

	if (iter != predefined_colors.end ())
		return iter->second;
	else if (colorStr[0] == '#')
	{
		uint8_t red, green, blue, alpha = 255;

		if (colorStr.length () - 1 == 3)
		{
			red = hex_char_to_int (colorStr[1]) * 16 + hex_char_to_int (colorStr[1]);
			green = hex_char_to_int (colorStr[2]) * 16 + hex_char_to_int (colorStr[2]);
			blue = hex_char_to_int (colorStr[3]) * 16 + hex_char_to_int (colorStr[3]);
		}
		else if (colorStr.length () - 1 == 4)
		{
			red = hex_char_to_int (colorStr[1]) * 16 + hex_char_to_int (colorStr[1]);
			green = hex_char_to_int (colorStr[2]) * 16 + hex_char_to_int (colorStr[2]);
			blue = hex_char_to_int (colorStr[3]) * 16 + hex_char_to_int (colorStr[3]);
			alpha = hex_char_to_int (colorStr[4]) * 16 + hex_char_to_int (colorStr[4]);
		}
		else if (colorStr.length () - 1 == 6)
		{
			red = hex_char_to_int (colorStr[1]) * 16 + hex_char_to_int (colorStr[2]);
			green = hex_char_to_int (colorStr[3]) * 16 + hex_char_to_int (colorStr[4]);
			blue = hex_char_to_int (colorStr[5]) * 16 + hex_char_to_int (colorStr[6]);
		}
		else if (colorStr.length () - 1 == 8)
		{
			red = hex_char_to_int (colorStr[1]) * 16 + hex_char_to_int (colorStr[2]);
			green = hex_char_to_int (colorStr[3]) * 16 + hex_char_to_int (colorStr[4]);
			blue = hex_char_to_int (colorStr[5]) * 16 + hex_char_to_int (colorStr[6]);
			alpha = hex_char_to_int (colorStr[7]) * 16 + hex_char_to_int (colorStr[8]);
		}

		return olc::Pixel (red, green, blue, alpha);
	}

	return olc::BLACK;
}

struct Colors
{
	Colors () {}

	Colors (ini::config conf)
	{
		ini::section& global_section = conf["global"];

		text = parse_color (global_section["foreground"]);
		background = parse_color (global_section["background"]);

		keywords = parse_color (global_section["keywords"]);
		operators = parse_color (global_section["operators"]);
		branching = parse_color (global_section["branching"]);
		looping = parse_color (global_section["looping"]);
		built_ins = parse_color (global_section["built_in_functions"]);
		types = parse_color (global_section["types"]);
		numbers = parse_color (global_section["numbers"]);
		identifiers = parse_color (global_section["identifiers"]);
		current_line_bg = parse_color (global_section["current_line_bg"]);
		line_number = parse_color (global_section["line_number_fg"]);
		line_number_bg = parse_color (global_section["line_number_bg"]);
	}

	olc::Pixel background;
	olc::Pixel keywords;
	olc::Pixel operators;
	olc::Pixel branching;
	olc::Pixel looping;
	olc::Pixel built_ins;
	olc::Pixel types;
	olc::Pixel numbers;
	olc::Pixel identifiers;
	olc::Pixel text;
	olc::Pixel current_line_bg;
	olc::Pixel line_number;
	olc::Pixel line_number_bg;
};

class display : public olc::PixelGameEngine
{
public:
	display ();
	~display ();
	std::shared_ptr <std::vector<std::string>> text_to_parse;

	inline void read_config ()
	{
		ini::config conf = ini::parse_file ("conf.ini");
		colors = Colors (conf);
	}

protected:
	bool OnUserCreate () override;
	bool OnUserUpdate (float deltaTime) override;
private:
	void input ();
	void render_text ();
	Colors colors;
};
