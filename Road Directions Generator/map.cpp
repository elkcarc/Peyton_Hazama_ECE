
#include "map.h"

std::vector<std::string> split(const char *str, char c = ' ')
{
	std::vector<std::string> result;
	do
	{
		const char *begin = str;
		while (*str != c && *str)
		{
			str++;
		}
		result.push_back(std::string(begin, str));
	} while (0 != *str++);
	return result;
}

connections::connections(){ }

connections::connections(std::string name, std::string code, int inter_a, int inter_b, double road_len)
{
	this->name = name;
	this->code = code;
	this->inter_a = inter_a;
	this->inter_b = inter_b;
	this->road_len = road_len;
}

void connections::print() const
{
	std::cout << this->name << ", " << this->road_len << " miles between location " << this->inter_a << " and location " << this->inter_b << ". (" << this->code << ")\n";
}

void connections::print(int code) const
{
	if (code == this->inter_a)
	{
		std::cout << this->name << ", " << this->road_len << " miles to " << this->inter_b << ". (" << this->code << ")\n";
	}
	else if (code == this->inter_b)
	{
		std::cout << this->name << ", " << this->road_len << " miles to " << this->inter_a << ". (" << this->code << ")\n";
	}
}

void connections::printdist() const
{
	std::cout << this->name << " " << this->road_len << " miles";
}

i_node::i_node(){ }

i_node::i_node(int line, double lon, double lat, double dist, std::string state, std::string name, std::vector<connections> &all_paths)
{
	this->line = line;
	this->lon = lon;
	this->lat = lat;
	this->dist = dist;
	this->state = state;
	this->name = name;
	this->fscore = -1;
	this->gscore = -1;
	this->parent = 0;
	//getConnections(this->paths, all_paths, line);
}

void i_node::getConnections(std::vector<connections*> &paths, std::vector<connections> &all_paths, int line)
{
	for (int i = 0; i < all_paths.size(); i++)
	{
		if (all_paths[i].inter_a == line || all_paths[i].inter_b == line)
		{
			paths.push_back(&all_paths[i]);
		}
	}
}

void i_node::print() const
{
	std::cout << "Location " << line << ", " << dist << " miles from " << name << ", " << state << "\n";
}

void i_node::printnodist() const
{
	std::cout << "Location " << line ;
}

void i_node::printpaths() const
{
	std::cout << "Location " << line << ", " << dist << " miles from " << name << ", " << state << "\n";
	for (int i = 0; i < paths.size(); i++)
	{
		paths[i]->print();
	}
}

void i_node::setf(int f)
{
	this->fscore = f;
}

void i_node::setg(int g)
{
	this->gscore = g;
}

map::map(){ }

map::map(std::string intersectionFile, std::string connectionsFile)
{
	create_map(intersectionFile, connectionsFile);
}

map::~map(){ }

void map::create_map(std::string intersectionFile, std::string connectionsFile)
{
	std::fstream fin1;
	std::string str;
	int i = 0;
	int j = 0;
	std::cout << connectionsFile.c_str() << "\n";
	fin1.open(connectionsFile.c_str());
	if (fin1.fail())
	{
		std::cerr << "Failed to open connections file." << std::endl;
	}
	else
	{
		while (std::getline(fin1, str))
		{
			while (str[i + j] != 32)
			{
				j++;
			}
			std::string name = str.substr(i, j);
			i = i + j + 1;
			j = 0;
			while (str[i + j] != 32)
			{
				j++;
			}
			std::string code = str.substr(i, j);
			i = i + j + 1;
			j = 0;
			while (str[i + j] != 32)
			{
				j++;
			}
			int inter_a = std::atoi(str.substr(i, j).c_str());
			i = i + j + 1;
			j = 0;
			while (str[i + j] != 32)
			{
				j++;
			}
			int inter_b = std::atoi(str.substr(i, j).c_str());
			i = i + j + 1;
			double road_len = std::atof(str.substr(i, std::string::npos).c_str());
			all_paths.push_back(connections(name, code, inter_a, inter_b, road_len));
			i = 0;
			j = 0;
		}
	}
	fin1.close();
	std::fstream fin2;
	std::cout << "Read in all roads" << "\n";
	std::cout << intersectionFile.c_str() << "\n";
	fin2.open(intersectionFile.c_str());
	if (fin2.fail())
	{
		std::cerr << "Failed to open intersections file." << std::endl;
	}
	else
	{
		while (std::getline(fin2, str))
		{
			int line = i;
			double lon = std::atof(str.substr(0, 9).c_str());
			double lat = std::atof(str.substr(10, 9).c_str());
			double dist = std::atof(str.substr(20, 8).c_str());
			std::string state = str.substr(28, 2);
			std::string name = str.substr(31, std::string::npos);
			i_node inter = i_node(line, lon, lat, dist, state, name, all_paths);
			i_map.push_back(inter);
			i++;
		}
		for(int i = 0; i < all_paths.size(); i++)
		{
			i_map[all_paths[i].inter_a].paths.push_back(&all_paths[i]);
			i_map[all_paths[i].inter_b].paths.push_back(&all_paths[i]);
		}
	}
	fin2.close();
}

i_node map::getnode(int code)
{
	return i_map[code];
}

void map::explore_map(int code)
{
	int x;
	int path;
	for (int i = 0; i < i_map.size(); i++)
	{
		if (code == i_map[i].line)
		{
			i_map[i].print();
			for (int j = 0; j < i_map[i].paths.size(); j++)
			{
				std::cout << j + 1 << ". ";
				i_map[i].paths[j]->print(i_map[i].line);
			}
			x = i;
			break;
		}
	}
	std::cout << "Select Path: ";
	std::cin >> path;
	if (path <= i_map[x].paths.size() && code == i_map[x].paths[path - 1]->inter_a && path > 0)
	{
		path = i_map[x].paths[path - 1]->inter_b;
		explore_map(path);
	}
	else if (path <= i_map[x].paths.size() && code == i_map[x].paths[path - 1]->inter_b && path > 0)
	{
		path = i_map[x].paths[path - 1]->inter_a;
		explore_map(path);
	}
	else
	{
		std::cout << "Invalid Path! Select Starting Location:  ";
		std::cin >> path;
		explore_map(path);
	}
}
 
std::vector<connections*> map::getNeighbors(std::vector<i_node> i_map, int code) 
{
    return i_map[code].paths;
}

double map::heuristic(int start, int end)
{
	i_node s = getnode(start);
	i_node e = getnode(end);
	return sqrt(std::pow(s.lon - e.lon, 2) + std::pow(s.lat - e.lat, 2));
}

int contained(std::vector<i_node*> closedSet, int x)
{
	for(int i = 0; i < closedSet.size(); i++)
	{
		if(closedSet[i]->line == x)
		{
			return 1;
		}
	}
	return 0;
}
 
i_node * map::AStar(int start, int goal)
{
	i_node test;
    std::vector<i_node*> closedSet;
    std::vector<i_node*> openSet;
	openSet.push_back(&i_map[start]);
	openSet[0]->gscore = 0;
	openSet[0]->fscore = heuristic(start, goal);
	double tempgscore;
    while (!openSet.empty()) 
	{
		i_node * current = new i_node;
		double ftest = 100000;
		for(int i = 0; i < openSet.size(); i++)
		{
			if(ftest > openSet[i]->fscore && openSet[i]->fscore >= 0)
			{
				ftest = openSet[i]->fscore;
				current = openSet[i];
			}
		}
		std::cout << "fscore: " << current->fscore << " ";
		current->print();
		if(current->line == goal)
		{
			return current;
		}
		openSet.erase(std::remove(openSet.begin(), openSet.end(), current ), openSet.end() );
		closedSet.push_back(current);
		for(int i = 0; i < current->paths.size(); i++)
		{
			if(contained(closedSet, current->paths[i]->inter_a) && contained(closedSet, current->paths[i]->inter_b))
			{
				continue;
			}
			else
			{
				tempgscore = current->gscore + current->paths[i]->road_len;
			}

			int flag = -1;
			if(!contained(openSet, current->paths[i]->inter_a) && contained(closedSet, current->paths[i]->inter_b))
			{
				openSet.push_back(&i_map[current->paths[i]->inter_a]);
				flag = 1;
			}
			else if(!contained(openSet, current->paths[i]->inter_b) && contained(closedSet, current->paths[i]->inter_a))
			{
				openSet.push_back(&i_map[current->paths[i]->inter_b]);
				flag = 2;
			}
			else if(tempgscore >= i_map[current->paths[i]->inter_a].gscore && i_map[current->paths[i]->inter_a].gscore >= 0 && flag == 1)
			{
				continue;
			}
			else if(tempgscore >= i_map[current->paths[i]->inter_b].gscore && i_map[current->paths[i]->inter_b].gscore >= 0 && flag == 2)
			{
				continue;
			}
			if(flag == 1)
			{
				i_map[current->paths[i]->inter_a].parent = current;
				i_map[current->paths[i]->inter_a].gscore = tempgscore;
				//i_map[current->paths[i]->inter_a].fscore = i_map[current->paths[i]->inter_a].gscore + heuristic(current->paths[i]->inter_a, goal); //shortest pathfind (not fast)
				i_map[current->paths[i]->inter_a].fscore = heuristic(current->paths[i]->inter_a, goal); //fast pathfind (not shortest)
			}
			if(flag == 2)
			{
				i_map[current->paths[i]->inter_b].parent = current;
				i_map[current->paths[i]->inter_b].gscore = tempgscore;
				//i_map[current->paths[i]->inter_b].fscore = i_map[current->paths[i]->inter_b].gscore + heuristic(current->paths[i]->inter_b, goal); //shortest pathfind (not fast)
				i_map[current->paths[i]->inter_b].fscore = heuristic(current->paths[i]->inter_b, goal); //fast pathfind (not shortest)
			}
		}
    }
	std::cout << "No path found\n";
	return 0;
}

void printangle(i_node * a, i_node * b)
{
	double x = a->lon - b->lon;
	double y = a->lat - b->lat;
	double angle = atan2(y, x) * (180 / 3.14159265);
	if(angle < 0)
	{
		angle = angle + 360;
	}
	if((angle >= 0 && angle <= 22.5) || angle > 347.5)
	{
		std::cout << " W ";
	}
	else if(angle <= 67.5 && angle > 22.5)
	{
		std::cout << " SW ";
	}
	else if(angle <= 112.5 && angle > 67.5)
	{
		std::cout << " S ";
	}
	else if(angle <= 157.5 && angle > 112.5)
	{
		std::cout << " SE ";
	}
	else if(angle <= 202.5 && angle > 157.5)
	{
		std::cout << " E ";	}
	else if(angle <= 247.5 && angle > 202.5)
	{
		std::cout << " NE ";
	}
	else if(angle <= 292.5 && angle > 247.5)
	{
		std::cout << " N ";
	}
	else
	{
		std::cout << " NW ";
	}
	std::cout << "(" << angle << " degrees) ";
}
 
int map::shortestPath(int s, int e) 
{
    i_node * path = AStar(s, e);
    if(path == 0)
    {
	exit(0);
    }
	while(1)
	{
		std::cout << "From ";
		path->printnodist();
		std::cout << " take ";
		for(int i = 0; i < path->paths.size(); i++)
		{
			if(path->paths[i]->inter_a == path->line && path->paths[i]->inter_b == path->parent->line)
			{
				path->paths[i]->printdist();
				printangle(path, path->parent);
				break;
			}
			else if(path->paths[i]->inter_b == path->line && path->paths[i]->inter_a == path->parent->line)
			{
				path->paths[i]->printdist();
				printangle(path, path->parent);
				break;
			}
		}
		std::cout << "to ";
		path->parent->printnodist();
		std::cout << "\n";
		path = path->parent;
		if(path->parent == 0)
		{
			break;
		}
	}
	return 0;
}



