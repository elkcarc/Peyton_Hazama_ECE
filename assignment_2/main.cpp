#define _GLIBCXX_USE_C99 1
#include <fstream>
#include <iostream>
#include "Hash.h"
#include <string>
#include <stdlib.h>

int main(void) 
{

	std::cout << "Loading..." << "\n";
	HashTable * h = new HashTable("named-places.txt");
	std::string line;
	std::cout << "Enter the name of a city. If the city is valid, enter the state the city is in to see details. Hit enter twice to exit." << "\n";
	do 
	{
		std::cout << "Enter City> ";
		std::getline(std::cin, line);
		if (line == "")
		{
			break;
		}
		PlaceVector pv = h->index(line);
		if (pv.length == 0) 
		{
			std::cout << "Invalid City Name!\n";
			continue;
		}
		pv.printStates();
		std::cout << "Enter State> ";
		std::getline(std::cin, line);
		pv.getState(line);
	}while (true);

	delete h;

	return 0;
}
