#include "display.hpp"
#include "parser.hpp"
#include "string_util.hpp"

#include <iostream>
#include <sstream>
#include <functional>

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
    console_buffer = std::make_unique<buffer>(256);
    current_buffer = text_to_parse.get();
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
    if(current_buffer == nullptr)
	return;
#define CHECK_KEY(key, not_shifted, shifted)				\
    do { if(GetKey((key)).bReleased) current_buffer->insert_at_cursor(shift ? (shifted) : (not_shifted)); } while(0)

    bool shift = GetKey(olc::SHIFT).bHeld;

    if(GetKey(olc::UP).bReleased)    current_buffer->up();
    if(GetKey(olc::DOWN).bReleased)  current_buffer->down();
    if(GetKey(olc::LEFT).bReleased)  current_buffer->left();
    if(GetKey(olc::RIGHT).bReleased) current_buffer->right();

    CHECK_KEY(olc::SPACE, ' ', ' ');
    if(GetKey(olc::BACK).bReleased) current_buffer->delete_at_cursor();
    if(GetKey(olc::DEL).bReleased)  current_buffer->delete_after_cursor();

    if(GetKey(olc::HOME).bReleased) current_buffer->seek_bol();
    if(GetKey(olc::END).bReleased)  current_buffer->seek_eol();

    for(int key = olc::A; key <= olc::Z; key++){
	olc::HWButton k = GetKey(key);
	if(k.bPressed)
	    current_buffer->insert_at_cursor((shift ? 'A' : 'a') + (key - olc::A));
    }

    const char* const chars = ")!@#$%^&*(";
    
    for(int key = olc::K0; key <= olc::K9; key++){
	olc::HWButton k = GetKey(key);
	if(k.bPressed)
	    current_buffer->insert_at_cursor((shift ? chars[key - olc::K0] : '0' + (key - olc::K0)));
    }

    CHECK_KEY(olc::BACKTICK, '`', '~');
    CHECK_KEY(olc::COMMA, ',', '<');
    CHECK_KEY(olc::SLASH, '/', '?');
    CHECK_KEY(olc::PERIOD, '.', '>');
    CHECK_KEY(olc::SEMICOLON, ';', ':');
    CHECK_KEY(olc::APOSTROPHE, '\'', '"');
    CHECK_KEY(olc::BACKSLASH, '\\', '|');
    CHECK_KEY(olc::LBRACKET, '[', '{');
    CHECK_KEY(olc::RBRACKET, ']', '}');
    CHECK_KEY(olc::MINUS, '-', '_');
    CHECK_KEY(olc::EQUAL, '=', '+');

#undef CHECK_KEY
}

struct Command {
    std::string name;
    std::string description;
    int numArgs;
    std::function<void(display&, std::string)> func;

    void operator()(display& d, std::string str) { func(d, str); }
};

static std::map<std::string, std::string> cmd_aliases =
{
    {"q", "quit"},
    {"fg", "foreground"},
    {"bg", "background"},
    {"lc", "load_config"},
};

static std::map<std::string, Command> commands =
{
    {"quit",
     {"quit", "Exits out of the program.", 0,
      [](display& d, std::string str){ d.exit(); }}
    },

    {"load_config",
     {"load_config", "Loads a configuration from a given file", 1,
      [](display& d, std::string str) { d.colors = Colors(ini::parse_file(str)); }}
    },

    {"foreground",
     { "foreground", "Does a thing", 1,
       [](display& d, std::string str) { d.colors.text = parse_color(str); }}
    },
    
    {"background",
     { "background", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.background = parse_color(str); }}
    },

    {"operators",
     { "operators", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.operators = parse_color(str); }}
    },

    {"branching",
     { "branching", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.branching = parse_color(str); }}
    },

    {"looping",
     { "looping", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.looping = parse_color(str); }}
    },

    {"built_ins",
     { "built_ins", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.built_ins = parse_color(str); }}
    },

    {"types",
     {"types", "Does a thing.", 1,
      [](display& d, std::string str) { d.colors.types = parse_color(str); }}
    },

    {"numbers",
     {"numbers", "Does a thing.", 1,
      [](display& d, std::string str) { d.colors.numbers = parse_color(str); }}},
    {"identifiers",
     { "identifiers", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.identifiers = parse_color(str); }}
    },

    {"text",
     {"text", "Does a thing.", 1,
      [](display& d, std::string str) { d.colors.text = parse_color(str); }}
    },

    {"current_line_bg",
     {"current_line_bg", "Does a thing.", 1,
      [](display& d, std::string str) { d.colors.current_line_bg = parse_color(str); }}
    },

    {"line_number",
     { "line_number", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.line_number = parse_color(str); }}
    },

    {"line_number_bg",
     { "line_number_bg", "Does a thing.", 1,
       [](display& d, std::string str) { d.colors.line_number_bg  = parse_color(str); }}
    },

    {"show_cursor_pos",
     { "show_cursor_pos", "Does a thing.", 1,
       [](display& d, std::string str) { d.show_cursor_pos = !d.show_cursor_pos; }}
    },
};

#include <chrono>
using Clock = std::chrono::steady_clock;

auto prevRender = Clock::now();
bool renderCursor = false;
bool ignore_input = false;
bool console_enabled = false;

void display::render_console()
{
    if(!console_enabled) {
	current_buffer = text_to_parse.get();
	return;
    }
    
    int console_height = 16;

    FillRect(0, ScreenHeight () - console_height,
	     ScreenWidth (), console_height,
	     parse_color("#111"));

    const std::string prompt = ":";
    const olc::Pixel prompt_color = parse_color("#7f7");

    // Render a prompt.
    DrawString(0, ScreenHeight () - 8 - console_height / 4, prompt, prompt_color);
    // Render the actual command.
    DrawString(prompt.length() * 8,
	       ScreenHeight() - 8 - console_height / 4,
	       *console_buffer,
	       colors.text);

    if(renderCursor)
	FillRect((prompt.length() + console_buffer->cursor_pos) * 8, ScreenHeight() - 8 - console_height / 4,
		 2, 8,
		 olc::WHITE);

    if(GetKey(olc::ENTER).bReleased) {
	auto arr = string_util::split_on_first(*console_buffer);
	std::cout << "arr[0]: \"" << arr[0] << "\", arr[1]: \"" << arr[1] << "\"\n";

	auto alias = cmd_aliases.find(arr[0]);
	auto iter = commands.find(arr[0]);
	if(iter != commands.end())
	{
	    std::cout << console_buffer->get_current_line() << '\n';
	    iter->second(*this, arr[1]);
	    should_render_console = false;
	    console_buffer->reset();
	    current_buffer = text_to_parse.get(); 
	}
	else if(alias != cmd_aliases.end() &&
		  commands.find(alias->second) != commands.end())
	{
	    std::cout << console_buffer->get_current_line() << '\n';
	    commands[alias->second](*this, arr[1]);
	    should_render_console = false;
	    console_buffer->reset();
	    current_buffer = text_to_parse.get(); 
	}
    }

    ignore_input = false;
}

void display::render_text ()
{
    int x_offset = std::to_string (text_to_parse->num_lines()).size () * 8 + line_num_margin;
	FillRect (0, cursor_line * 8, ScreenWidth (), 8, colors.current_line_bg);
	FillRect (0, 0, x_offset - cursor_row * 8, ScreenHeight (), colors.line_number_bg);


    if(renderCursor)
	FillRect(x_offset + text_to_parse->get_cursor_x() * 8, text_to_parse->get_cursor_y() * 8,
		 2, 8,
		 olc::WHITE);

	auto v = text_to_parse->as_lines_vec();

	for (int line = 0; line < v.size(); ++line)
	{
		DrawString (line_num_margin / 2 - cursor_row * 8, line * 8, std::to_string (line), colors.line_number);
		for (const auto& token : parse (v.at (line)))
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

	char c = text_to_parse->char_under_cursor();

	std::stringstream ss;
	ss << "cursor position: x = " << text_to_parse->get_cursor_x()
	   << ", y = " << text_to_parse->get_cursor_y()
	   << " (cursor_pos: " << text_to_parse->cursor_pos
	   << ", under cursor: " << (c == '\n' ? "\\n" : std::to_string((char)c)) << ")";
	// ss << text_to_parse->get_current_line();

	DrawString(ScreenWidth() / 2 - (ss.str().length() * 8 / 2), ScreenHeight() - 8, ss.str());

    if(should_render_console)
	render_console();
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

bool display::OnUserUpdate (float deltaTime)
{
    current_buffer = text_to_parse.get(); 

    if(std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - prevRender).count() > 350) {
	renderCursor = !renderCursor;
	prevRender = Clock::now();
    }

    if(GetKey(olc::SEMICOLON).bReleased
       && GetKey(olc::SHIFT).bHeld
       && !should_render_console)
    {
	should_render_console = true;
	current_buffer = console_buffer.get();
    }
    if(GetKey(olc::ESCAPE).bReleased
       && should_render_console)
    {
	should_render_console = false;
	console_buffer->delete_line();
	current_buffer = text_to_parse.get(); 
    }
    if(current_buffer == text_to_parse.get()
       && current_buffer != nullptr
       && GetKey(olc::ENTER).bReleased)
	text_to_parse->insert_at_cursor('\n');

    input();

    Clear (colors.background);
    render_text ();
    return true;
}
