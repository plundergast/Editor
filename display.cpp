#include "display.hpp"
#include "parser.hpp"

#include <sstream>

const std::map<std::string, olc::Pixel> predefined_colors = {
	{ "white", olc::WHITE },
	{ "gray", olc::GREY },
	{ "grey", olc::GREY },
	{ "dark gray", olc::DARK_GREY },
	{ "dark grey", olc::DARK_GREY },
	{ "darkgray", olc::DARK_GREY },
	{ "darkgrey", olc::DARK_GREY },
	{ "dark_gray", olc::DARK_GREY },
	{ "dark_grey", olc::DARK_GREY },
	{ "very dark gray", olc::VERY_DARK_GREY },
	{ "very dark grey", olc::VERY_DARK_GREY },
	{ "verydarkgray", olc::VERY_DARK_GREY },
	{ "verydarkgrey", olc::VERY_DARK_GREY },
	{ "very_dark_gray", olc::VERY_DARK_GREY },
	{ "very_dark_grey", olc::VERY_DARK_GREY },
	{ "red", olc::RED },
	{ "dark red", olc::DARK_RED },
	{ "darkred", olc::DARK_RED },
	{ "dark_red", olc::DARK_RED },
	{ "very dark red", olc::VERY_DARK_RED },
	{ "verydarkred", olc::VERY_DARK_RED },
	{ "very_dark_red", olc::VERY_DARK_RED },
	{ "yellow", olc::YELLOW },
	{ "dark yellow", olc::DARK_YELLOW },
	{ "darkyellow", olc::DARK_YELLOW },
	{ "dark_yellow", olc::DARK_YELLOW },
	{ "very dark yellow", olc::VERY_DARK_YELLOW },
	{ "verydarkyellow", olc::VERY_DARK_YELLOW },
	{ "very_dark_yellow", olc::VERY_DARK_YELLOW },
	{ "green", olc::GREEN },
	{ "dark green", olc::DARK_GREEN },
	{ "darkgreen", olc::DARK_GREEN },
	{ "dark_green", olc::DARK_GREEN },
	{ "very dark green", olc::VERY_DARK_GREEN },
	{ "verydarkgreen", olc::VERY_DARK_GREEN },
	{ "very_dark_green", olc::VERY_DARK_GREEN },
	{ "cyan", olc::CYAN },
	{ "dark cyan", olc::DARK_CYAN },
	{ "darkcyan", olc::DARK_CYAN },
	{ "dark_cyan", olc::DARK_CYAN },
	{ "very dark cyan", olc::VERY_DARK_CYAN },
	{ "verydarkcyan", olc::VERY_DARK_CYAN },
	{ "very_dark_cyan", olc::VERY_DARK_CYAN },
	{ "blue", olc::BLUE },
	{ "dark blue", olc::DARK_BLUE },
	{ "darkblue", olc::DARK_BLUE },
	{ "dark_blue", olc::DARK_BLUE },
	{ "very dark blue", olc::VERY_DARK_BLUE },
	{ "verydarkblue", olc::VERY_DARK_BLUE },
	{ "very_dark_blue", olc::VERY_DARK_BLUE },
	{ "magenta", olc::MAGENTA },
	{ "dark magenta", olc::DARK_MAGENTA },
	{ "darkmagenta", olc::DARK_MAGENTA },
	{ "dark_magenta", olc::DARK_MAGENTA },
	{ "very dark magenta", olc::VERY_DARK_MAGENTA },
	{ "verydarkmagenta", olc::VERY_DARK_MAGENTA },
	{ "very_dark_magenta", olc::VERY_DARK_MAGENTA },
	{ "black", olc::BLACK },
	{ "blank", olc::BLANK },
};

display::display ()
{
	read_config ();
}

display::~display ()
{}

bool display::OnUserCreate ()
{
	return true;
}

void display::input ()
{
	// Note: I don't know how to do this.
	// if(GetKey(olc::R).bReleased) {
	// 	ReadConfig();
	// 	colors.Background = olc::RED;
	// }
}

void display::render_text ()
{
	int x_offset = std::to_string (text_to_parse->size ()).size () * 8 + line_num_margin;
	FillRect (0, cursor_line * 8, ScreenWidth (), 8, colors.current_line_bg);
	FillRect (0, 0, x_offset - cursor_row * 8, ScreenHeight (), colors.line_number_bg);

	for (int line = 0; line < text_to_parse->size (); ++line)
	{
		DrawString (line_num_margin / 2 - cursor_row * 8, line * 8, std::to_string (line), colors.line_number);
		for (const auto& token : parse (text_to_parse->at (line)))
		{
			olc::Pixel color = olc::BLACK;
			switch (token.type)
			{
			case Token::Type::Keyword:           color = colors.keywords;    break;
			case Token::Type::Operator:          color = colors.operators;   break;
			case Token::Type::Branching:         color = colors.branching;   break;
			case Token::Type::Looping:           color = colors.looping;     break;
			case Token::Type::Built_In_Function: color = colors.built_ins;   break;
			case Token::Type::Type:              color = colors.types;       break;
			case Token::Type::Number:            color = colors.numbers;     break;
			case Token::Type::Identifier:        color = colors.identifiers; break;
			case Token::Type::Text:              color = colors.text;        break;
			}
			DrawString ((token.location - cursor_row) * 8 + x_offset, line * 8, token.text, color);
		}
	}

	std::stringstream ss;
	ss << "cursor position: x = " << cursor_row << ", y = " << cursor_line;

	DrawString(ScreenWidth() / 2 - (ss.str().length() * 8 / 2), ScreenHeight() - 8, ss.str());
}

template<typename IterT>
static unsigned int longest_string(IterT begin, IterT end) {
    unsigned int max_len = 0;

    while(begin != end)
    {
	if(begin->length() > max_len)
	    max_len = begin->length();

	begin++;
    }

    return max_len;
}

#include <iostream>

bool display::OnUserUpdate (float deltaTime)
{
    olc::HWButton down = GetKey(olc::DOWN);
    olc::HWButton up = GetKey(olc::UP);

    olc::HWButton shift = GetKey(olc::SHIFT);

    if(up.bReleased)
	cursor_line = std::max<int>(cursor_line - (shift.bHeld ? 10 : 1), 0);
    else if(down.bReleased)
	cursor_line = std::min<int>(cursor_line + (shift.bHeld ? 10 : 1), text_to_parse->size() - 1);

    olc::HWButton left = GetKey(olc::LEFT);
    olc::HWButton right = GetKey(olc::RIGHT);

    if(left.bReleased)
	cursor_row = std::max<int>(cursor_row - (shift.bHeld ? 10 : 1), 0);
    else if(right.bReleased)
	cursor_row = std::min<int>(cursor_row + (shift.bHeld ? 10 : 1),
				   longest_string(text_to_parse->begin(), text_to_parse->end())
				   - (ScreenWidth() - line_num_margin - 8) / 8 + 5);

	Clear (colors.background);
	render_text ();
	return true;
}
