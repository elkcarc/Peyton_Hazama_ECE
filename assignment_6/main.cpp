#include <fstream>
#include <iostream>
#include "hash.h"
#include "map.h"
#include <string>
#include <stdlib.h>


int main() 
{
	int start = -1;
	int end = -1;
	std::cout << "Loading..." << "\n";
	HashTable * h = new HashTable("named-places.txt");
	std::string line;
	std::cout << "Enter the name of a city. If the city is valid, enter the state the city is in to see details. Hit enter twice to exit." << "\n";
	do 
	{
		if(start == -1)
		{
			std::cout << "Enter Start> ";
		}
		else
		{
			std::cout << "Enter Destination> ";
		}
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
		if(start == -1)
		{			
			start = pv.getState(line);
			std::cout << "Location: " << start << "\n";
		}
		else if (end == -1)
		{
			end = pv.getState(line);
			std::cout << "Location: " << end << "\n";
		}
	}while (start == -1 || end == -1);

	delete h;
	
	std::cout << "Reading in USA map" << "\n";
	map * USA = new map("intersections.txt", "connections.txt");
	
	USA->shortestPath(end, start);

	return 0;
}
