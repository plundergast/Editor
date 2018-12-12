#include "display.h"
#include "parser.h"

std::map<std::string, olc::Pixel> PredefinedColors = {
    {"white", olc::WHITE},

    {"gray", olc::GREY},
    {"grey", olc::GREY},

    {"dark gray", olc::DARK_GREY},
    {"dark grey", olc::DARK_GREY},
    {"darkgray",  olc::DARK_GREY},
    {"darkgrey",  olc::DARK_GREY},
    {"dark_gray", olc::DARK_GREY},
    {"dark_grey", olc::DARK_GREY},

    {"very dark gray", olc::VERY_DARK_GREY},
    {"very dark grey", olc::VERY_DARK_GREY},
    {"verydarkgray",   olc::VERY_DARK_GREY},
    {"verydarkgrey",   olc::VERY_DARK_GREY},
    {"very_dark_gray", olc::VERY_DARK_GREY},
    {"very_dark_grey", olc::VERY_DARK_GREY},

    {"red", olc::RED}, 

    {"dark red", olc::DARK_RED}, 
    {"darkred",  olc::DARK_RED}, 
    {"dark_red", olc::DARK_RED}, 

    {"very dark red", olc::VERY_DARK_RED},
    {"verydarkred",   olc::VERY_DARK_RED},
    {"very_dark_red", olc::VERY_DARK_RED},

    {"yellow", olc::YELLOW}, 

    {"dark yellow", olc::DARK_YELLOW}, 
    {"darkyellow",  olc::DARK_YELLOW}, 
    {"dark_yellow", olc::DARK_YELLOW}, 

    {"very dark yellow", olc::VERY_DARK_YELLOW},
    {"verydarkyellow",   olc::VERY_DARK_YELLOW},
    {"very_dark_yellow", olc::VERY_DARK_YELLOW},

    {"green", olc::GREEN}, 

    {"dark green", olc::DARK_GREEN}, 
    {"darkgreen",  olc::DARK_GREEN}, 
    {"dark_green", olc::DARK_GREEN}, 

    {"very dark green", olc::VERY_DARK_GREEN},
    {"verydarkgreen",   olc::VERY_DARK_GREEN},
    {"very_dark_green", olc::VERY_DARK_GREEN},

    {"cyan", olc::CYAN}, 

    {"dark cyan", olc::DARK_CYAN}, 
    {"darkcyan",  olc::DARK_CYAN}, 
    {"dark_cyan", olc::DARK_CYAN}, 

    {"very dark cyan", olc::VERY_DARK_CYAN},
    {"verydarkcyan",   olc::VERY_DARK_CYAN},
    {"very_dark_cyan", olc::VERY_DARK_CYAN},

    {"blue", olc::BLUE}, 

    {"dark blue", olc::DARK_BLUE}, 
    {"darkblue",  olc::DARK_BLUE}, 
    {"dark_blue", olc::DARK_BLUE}, 

    {"very dark blue", olc::VERY_DARK_BLUE},
    {"verydarkblue",   olc::VERY_DARK_BLUE},
    {"very_dark_blue", olc::VERY_DARK_BLUE},

    {"magenta", olc::MAGENTA}, 

    {"dark magenta", olc::DARK_MAGENTA}, 
    {"darkmagenta",  olc::DARK_MAGENTA}, 
    {"dark_magenta", olc::DARK_MAGENTA}, 

    {"very dark magenta", olc::VERY_DARK_MAGENTA},
    {"verydarkmagenta",   olc::VERY_DARK_MAGENTA},
    {"very_dark_magenta", olc::VERY_DARK_MAGENTA},

    {"black", olc::BLACK},

    {"blank", olc::BLANK},
};

display::display ()
{
    ReadConfig();
}

display::~display ()
{
}

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
    int lineNumMargin = 12;

    int x_offset = std::to_string(text_to_parse->size()).size() * 8 + lineNumMargin;
    FillRect (0, 32, ScreenWidth(), 8, colors.CurrentLineBg);
    FillRect (0, 0, x_offset, ScreenHeight(), colors.LineNumberBg);
	
    for (int line = 0; line < text_to_parse->size (); ++line)
    {	
	DrawString (lineNumMargin / 2, line * 8, std::to_string (line), colors.LineNumber);
	for (const auto& token : parse (text_to_parse->at (line)))
	{
	    olc::Pixel color = olc::BLACK;
	    switch (token.type)
	    {
	    case Token::Type::Keyword:           color = colors.Keywords;    break;
	    case Token::Type::Operator:          color = colors.Operators;   break;
	    case Token::Type::Branching:         color = colors.Branching;   break;
	    case Token::Type::Looping:           color = colors.Looping;     break;
	    case Token::Type::Built_In_Function: color = colors.BuiltIns;    break;
	    case Token::Type::Type:              color = colors.Types;       break;
	    case Token::Type::Number:            color = colors.Numbers;     break;
	    case Token::Type::Identifier:        color = colors.Identifiers; break;
	    case Token::Type::Text:              color = colors.Text;        break;
	    }
	    DrawString(token.location * 8 + x_offset, line * 8, token.text, color);
	}
    }
}

bool display::OnUserUpdate (float deltaTime)
{
    Clear(colors.Background);
    render_text();
    return true;
}
