#include "display.h"
#include "parser.h"

display::display ()
{
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
}

void display::render_text ()
{
	int x_offset = std::to_string(text_to_parse->size()).size() * 8;
	FillRect (0, 32, ScreenWidth (), 8, olc::Pixel (230, 230, 230));
	FillRect (0, 0, x_offset, ScreenHeight (), olc::Pixel(225,225,225));
	
	for (int line = 0; line < text_to_parse->size (); ++line)
	{	
		DrawString (4, line * 8, std::to_string (line), olc::DARK_BLUE);
		for (const auto& token : parse (text_to_parse->at (line)))
		{
			olc::Pixel color = olc::BLACK;
			switch (token.type)
			{
			case Token::Type::Keyword:
				color = olc::DARK_CYAN;
				break;
			case Token::Type::Operator:
				color = olc::DARK_MAGENTA;
				break;
			case Token::Type::Branching:
				color = olc::DARK_BLUE;
				break;
			case Token::Type::Looping:
				color = olc::RED;
				break;
			case Token::Type::Built_In_Function:
				color = olc::DARK_YELLOW;
				break;
			case Token::Type::Type:
				color = olc::GREY;
				break;
			case Token::Type::Number:
				color = olc::DARK_RED;
				break;
			}
			DrawString (token.location * 8 + x_offset, line * 8, token.text, color);
		}
	}
}


bool display::OnUserUpdate (float deltaTime)
{
	Clear (olc::WHITE);
	render_text ();
	return true;
}
