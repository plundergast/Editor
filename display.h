#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <memory>
#include "StringUtil.hpp"
#include "INI.hpp"

extern std::map<std::string, olc::Pixel> PredefinedColors;

static inline int HexCharToInt(char hex)
{
    if(hex >= '0' && hex <= '9')
	return hex - '0';
    else {
	if(hex >= 'A' && hex <= 'Z') {
	    return hex - 'A' + 10;
	} else if (hex >= 'a' && hex <= 'z') {
	    return hex - 'a' + 10;
	}
    }

    return -1;
}

static inline olc::Pixel ParseColor(std::string colorStr)
{
    colorStr = Util::ToLower(Util::Trim(colorStr));
    auto iter = PredefinedColors.find(colorStr);

    if(iter != PredefinedColors.end())
	return iter->second;
    else if(colorStr[0] == '#')
    {
	uint8_t red, green, blue, alpha = 255;

	if(colorStr.length() - 1 == 3)
	{
	    red   = HexCharToInt(colorStr[1]) * 16 + HexCharToInt(colorStr[1]);
	    green = HexCharToInt(colorStr[2]) * 16 + HexCharToInt(colorStr[2]);
	    blue  = HexCharToInt(colorStr[3]) * 16 + HexCharToInt(colorStr[3]);
	} else if(colorStr.length() - 1 == 4) {
	    red   = HexCharToInt(colorStr[1]) * 16 + HexCharToInt(colorStr[1]);
	    green = HexCharToInt(colorStr[2]) * 16 + HexCharToInt(colorStr[2]);
	    blue  = HexCharToInt(colorStr[3]) * 16 + HexCharToInt(colorStr[3]);
	    alpha = HexCharToInt(colorStr[4]) * 16 + HexCharToInt(colorStr[4]);
	} else if(colorStr.length() - 1 == 6) {
	    red   = HexCharToInt(colorStr[1]) * 16 + HexCharToInt(colorStr[2]);
	    green = HexCharToInt(colorStr[3]) * 16 + HexCharToInt(colorStr[4]);
	    blue  = HexCharToInt(colorStr[5]) * 16 + HexCharToInt(colorStr[6]);
	} else if(colorStr.length() - 1 == 8) {
	    red   = HexCharToInt(colorStr[1]) * 16 + HexCharToInt(colorStr[2]);
	    green = HexCharToInt(colorStr[3]) * 16 + HexCharToInt(colorStr[4]);
	    blue  = HexCharToInt(colorStr[5]) * 16 + HexCharToInt(colorStr[6]);
	    alpha = HexCharToInt(colorStr[7]) * 16 + HexCharToInt(colorStr[8]);
	}

	return olc::Pixel(red, green, blue, alpha);
    }
	
    return olc::BLACK;
}

struct Colors {
    Colors() {}

    Colors(INI::Config conf){
	INI::Section& s = conf["global"];

	Text = ParseColor(s["foreground"]);
	Background = ParseColor(s["background"]);

	Keywords      = ParseColor(s["keywords"]);
	Operators     = ParseColor(s["operators"]);
	Branching     = ParseColor(s["branching"]);
	Looping       = ParseColor(s["looping"]);
	BuiltIns      = ParseColor(s["built_in_functions"]);
	Types         = ParseColor(s["types"]);
	Numbers       = ParseColor(s["numbers"]);
	Identifiers   = ParseColor(s["identifiers"]);
	CurrentLineBg = ParseColor(s["current_line_bg"]);
	LineNumber    = ParseColor(s["line_number_fg"]);
	LineNumberBg  = ParseColor(s["line_number_bg"]);
    }

    olc::Pixel Background;

    olc::Pixel Keywords;
    olc::Pixel Operators;
    olc::Pixel Branching;
    olc::Pixel Looping;
    olc::Pixel BuiltIns;
    olc::Pixel Types;
    olc::Pixel Numbers;
    olc::Pixel Identifiers;
    olc::Pixel Text;
    olc::Pixel CurrentLineBg;

    olc::Pixel LineNumber;
    olc::Pixel LineNumberBg;
};

class display : public olc::PixelGameEngine
{
public:
    display();
    ~display();
    std::shared_ptr <std::vector<std::string>> text_to_parse;

    inline void ReadConfig() {
	INI::Config conf = INI::ParseFile("conf.ini");
	colors = Colors(conf);
    }
protected:
    bool OnUserCreate () override;
    bool OnUserUpdate (float deltaTime) override;
private:
    void input ();
    void render_text ();
    Colors colors;
};
