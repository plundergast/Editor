#pragma once

#include <vector>
#include <string>

struct Token
{
	int location;
	std::string text;
	enum class Type : int
	{
		Text = 0,
		Keyword = 1,
		Identifier = 2,
		Branching = 3,
		Looping= 4,
		Built_In_Function = 5,
		Type = 6,
		Operator = 7,
		Number
	} type;
	Token (int location, const std::string& text, Type type);
};

std::vector<Token> parse (const std::string text);

const std::vector<std::string> keywords =
{
	"def",
	"return",
	"class",
};

const std::vector<std::string> identifiers =
{
	"self",
};

const std::vector<std::string> branching =
{
	"if",
	"else",
	"elif",
	"continue",
	"break",
	"yield"
};

const std::vector<std::string> looping =
{
	"for",
	"while",
	"in"
};

const std::vector<std::string> built_in_functions =
{
	"print",
	"list",
	"input",
	"len",
	"range"
};