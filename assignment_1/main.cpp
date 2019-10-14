#include <iostream>
#include "tree.h"

void main()
{
	std::string word;
	tree dictionary;
	int n = 0;
	std::cout << "Type a word or multiple words separated by spaces and press enter. When you are done entering words, enter '*'." << std::endl;
	while (true)
	{
		std::cin >> word;
		if (word == "*")
		{
			break;
		}
		int n = dictionary.addString(word);
		if(n == 1)
		{
		std::cout << n << " node in tree." << std::endl;
		}
		else if(n > 1)
		{
		std::cout << n << " nodes in tree." << std::endl;
		}
	}
	//dictionary.printTree();
	std::cout << "Search for a word or multiple words separated by spaces and press enter. When you are done searching for words, enter '*'." << std::endl;
	while (true)
	{
		std::cin >> word;
		if (word == "*")
		{
			break;
		}
		dictionary.searchTree(word);
	}
}
