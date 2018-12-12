
#include <iostream>

//#define debug_the_parser

#ifdef debug_the_parser
#include "parser.h"
#else
#include "display.h"
#endif

int main ()
{
    std::shared_ptr<std::vector<std::string>> text
	(
	    std::make_shared<std::vector<std::string>>
	    (
		std::vector<std::string>({
			"class Node:",
			"    def __init__(self, data):",
			"        self.data = data",
			"        self.left = None",
			"        self.right = None",
			"",
			"def depth_of_tree(tree):",
			"    if tree is None:",
			"        return 0",
			"    else:",
			"        depth_l_tree = depth_of_tree(tree.left)",
			"        depth_r_tree = depth_of_tree(tree.right)",
			"        if depth_l_tree > depth_r_tree:",
			"            return 1 + depth_l_tree",
			"        else:",
			"            return 1 + depth_r_tree",
			"",
			"def is_full_binary_tree(tree):",
			"    if tree is None:",
			"        return True",
			"    if (tree.left is None) and (tree.right is None):",
			"        return True",
			"    if (tree.left is not None) and (tree.right is not None):",
			"        return (is_full_binary_tree(tree.left) and is_full_binary_tree(tree.right))",
			"    else:",
			"        return False",
			"",
			"def main():",
			"    tree = Node(1)",
			"    tree.left = Node(2)",
			"    tree.right = Node(3)",
			"    tree.left.left = Node(4)",
			"    tree.left.right = Node(5)",
			"    tree.left.right.left = Node(6)",
			"    tree.right.left = Node(7)",
			"    tree.right.left.left = Node(8)",
			"    tree.right.left.left.right = Node(9)",
			"",
			"    print(is_full_binary_tree(tree))",
			"    print(depth_of_tree(tree))",
			"",
			"if __name__ == '__main__':",
			"    main()"
		    })
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
