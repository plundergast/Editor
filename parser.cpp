#include "parser.hpp"

Token::Token (int location, const std::string& text, Type type) :
	location{ location },
	text{ text },
	type{ type }
{
}

inline const bool is_operator (const char& character)
{
	switch (character)
	{
		case '+':
		case '-':
		case '*':
		case '/':
		case '(':
		case ')':
		case ':':
		case '>':
		case '<':
		case '=':
			return true;
		default:
			return false;
	}
}

inline const bool is_delimiter (const char& character)
{
	switch (character)
	{
		case ';':
		case '.':
		case ',':
			return true;
		default:
			return false;
	}
}

inline const bool is_space (const char& character)
{
	return character == ' ';
}

inline const bool is_comment_beginning (const char& character)
{
	return character == '#';
}

inline const bool is_newline (const char& character)
{
	return character == '\n';
}

inline const bool is_comment_end (const char& character)
{
	return is_newline (character);
}

inline const bool is_string_toggle (const char& character)
{
	return character == '\"' || character == '\'';
}

inline const bool is_hidden (const char& character)
{
	return character == '\t' || is_newline (character);
}

inline const bool is_number (const char& character)
{
	switch (character)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
	}
	return false;
}

inline const bool is_appendable (const char& character)
{
	return !is_number (character) && !is_hidden (character) && !is_operator (character) && !is_delimiter (character) && !is_space (character) || is_string_toggle (character);
}

void push_token (const std::string& text, int beginning, std::vector<Token>& out)
{
	Token::Type type = Token::Type::Text;
	bool set_type = false;

	for (const auto& keyword : keywords)
	{
		if (text == keyword)
		{
			type = Token::Type::Keyword;
			set_type = true;
			break;
		}
	}

	if (!set_type)
		for (const auto& identifier : identifiers)
		{
			if (text == identifier)
			{
				type = Token::Type::Identifier;
				set_type = true;
				break;
			}
		}

	if (!set_type)
		for (const auto& branch : branching)
		{
			if (text == branch)
			{
				type = Token::Type::Branching;
				set_type = true;
				break;
			}
		}

	if (!set_type)
		for (const auto& loop : looping)
		{
			if (text == loop)
			{
				type = Token::Type::Looping;
				set_type = true;
				break;
			}
		}

	if (!set_type)
		for (const auto& builtin : built_in_functions)
		{
			if (text == builtin)
			{
				type = Token::Type::Built_In_Function;
				set_type = true;
				break;
			}
		}

	out.push_back (Token (beginning, text, type));
}

void push_singular_token (std::string& word, const	char& character, int old_beginning, int character_distance, Token::Type type, bool& inside_text, std::vector<Token>& tokens)
{
	push_token (word, old_beginning, tokens);
	word.clear ();
	word.push_back (character);
	tokens.push_back (Token (character_distance, word, type));
	word.clear ();
	inside_text = false;
}

std::vector<Token> parse (const std::string text)
{
	std::vector<Token> tokens;
	std::string::const_iterator string_reader = text.begin ();
	std::string::const_iterator string_end = text.end ();
	std::string word;

	bool inside_text = false;
	bool inside_string = false;
	int token_beginning = 0;
	int tabbing = 0;

	while (string_reader < string_end)
	{
		const char& character = *string_reader;
		const bool char_is_operator = is_operator (character);
		const bool char_is_delimiter = is_delimiter (character);
		const bool char_is_space = is_space (character);
		const bool char_is_appendable = is_appendable (character);
		const bool char_is_number = is_number (character);
		
		if (character == '\t')
			tabbing += 3;

		const int character_distance = std::distance (text.begin (), string_reader) + tabbing;

		if (char_is_number)
		{
			push_singular_token (word, character, token_beginning, character_distance, Token::Type::Number, inside_text, tokens);
		}
		else if (char_is_operator || char_is_delimiter || char_is_number)
		{
			push_singular_token (word, character, token_beginning, character_distance, Token::Type::Operator, inside_text, tokens);
		}
		else if (char_is_appendable)
		{
			if (!inside_text)
			{
				// push tokens
				token_beginning = character_distance;
				inside_text = true;
			}
			word.push_back (character);
		}

		// spaces split 'delimit' the words
		else if (char_is_space)
		{
			push_token (word, token_beginning, tokens);
			word.clear ();
			inside_text = false;
		}

		++string_reader;

		if (is_appendable && string_reader == string_end)
		{
			push_token (word, token_beginning, tokens);
			word.clear ();
			inside_text = false;
		}
	}

	return tokens;
}
