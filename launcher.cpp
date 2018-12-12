
#include <iostream>

//#define debug_the_parser

#ifdef debug_the_parser
#include "parser.h"
#else
#include "display.h"
#endif
int main ()
{
	std::shared_ptr<std::vector<std::string>> text (std::make_shared<std::vector<std::string>> (
		std::vector<std::string>(
			{
				"def evens(count):",
				"	for i in range (0,count,1):",
				"		if i % 2 == 0:",
				"			print(\"even\")",
				"		else:",
				"			print(\"odd\")",
				"	return count / 2",
				"",
				"for i in range(100):",
				"	evens(i)",
				"	evens(i * i)"
		}
		)
	));
#ifdef debug_the_parser
	for(int line = 0; line < text->size(); ++line)
	for (const auto& token : parse (text->at(line)))
	{
		std::cout << token.location << '\t' << token.text << " " << std::to_string((int)token.type) << '\n';
	}
	std::cin.ignore ();
#else
	display surface;
	if (surface.Construct (640, 480, 1, 1))
	{
		surface.text_to_parse = text;
		surface.Start ();
		return EXIT_SUCCESS;
	}
	else return EXIT_FAILURE;
#endif
}