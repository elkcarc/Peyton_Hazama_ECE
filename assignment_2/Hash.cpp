#include "Hash.h"

unsigned long hash(std::string s, const unsigned long &len)
{
	unsigned long h = 78561;
	for (int i = 0; i < s.length(); i += 1)
	{
		h = (h * 691) + s[i];
	}
	h = h % len;
	return h;
}

Place::Place() {}

Place::Place(
	int			numeric_code,
	std::string	state_abr,
	std::string	place_name,
	int			population,
	double		area,
	double		lat,
	double		lon,
	int			r_i_code,
	double		int_dist) 
{
		this->numeric_code = numeric_code;
		this->state_abr = state_abr;
		this->place_name = place_name;
		this->population = population;
		this->area = area;
		this->lat = lat;
		this->lon = lon;
		this->r_i_code = r_i_code;
		this->int_dist = int_dist;
}

void Place::print() const 
{
	std::cout << std::setw(8) << std::setfill('0') << numeric_code << " " << state_abr << " " << place_name;
	std::cout << "\t" << population << "\t";
	std::cout << std::fixed;
	std::cout.precision(6);
	std::cout << area << "\t" << lat << "\t" << lon << "\t" << r_i_code << "\t" << int_dist << "\n";
}

PlaceVector::PlaceVector() 
{
	this->length = 0;
	this->absolute_length = 4;
	parray = new Place[this->absolute_length];
}

void PlaceVector::add(Place p) 
{
	if (this->length < this->absolute_length - 1) 
	{
		parray[this->length] = p;
	} 
	else 
	{
		this->absolute_length *= 2;
		Place * new_array = new Place[this->absolute_length];
		for (size_t i = 0; i < this->length; i++)
		{
			new_array[i] = parray[i];
		}
		delete[] parray;
		parray = new_array;
		parray[this->length] = p;
	}
	this->length++;
}

void PlaceVector::printStates() const 
{
	std::string states = "";
	for (size_t i = 0; i < this->length; i++)
	{
		std::string state = parray[i].state_abr;
		bool flag = false;
		for (int j = 0; j < states.length(); j+=2)
		{
			if (states.substr(j, 2) == state)
			{
				flag = true;
			}
		}
		if (!flag)
		{
			states += state;
			std::cout << state << std::endl;
		}
	}
}

void PlaceVector::getState(const std::string &st) const 
{
	std::string st_uprcs = st;
	for (int k = 0; k < 2; k++)
	{
		if (st[k] > 90)
		{
			st_uprcs[k] = st[k] - 32;
		}
	}

	bool flag = true;
	for (size_t i = 0; i < this->length; i++)
	{
		if (parray[i].state_abr == st_uprcs) 
		{
			parray[i].print();
			flag = false;
		}
	}
	if (flag)
	{
		std::cout << "Invalid State Abbreviation!" << "\n";
	}
}

HashTable::HashTable(std::string filename) 
{
	this->v = new PlaceVector[100000];
	std::fstream fin;
	fin.open(filename.c_str());
	if (fin.fail())
	{
		std::cerr << "Failed to open file." << std::endl;
	} 
	else 
	{
		int i = 0;
		std::string line;
		while (std::getline(fin, line)) 
		{
			int code = std::atoi(line.substr(0, 8).c_str());
			std::string state_abr = line.substr(8, 2);
			std::string whitespace = line.substr(10, 45);
			i = 44;
			while (whitespace[i] == ' ')
			{
				i--;
			}
			i++;
			std::string name = whitespace.substr(0, i);
			int population = std::atoi(line.substr(50, 16).c_str());
			double area = std::atof(line.substr(66, 14).c_str());
			double lat = std::atof(line.substr(80, 10).c_str());
			double lon = std::atof(line.substr(90, 11).c_str());
			int r_i_code = std::atoi(line.substr(101, 5).c_str());
			double int_dist = std::atof(line.substr(106).c_str());
			Place p = Place(code, state_abr, name, population, area, lat, lon, r_i_code, int_dist);
			index(name).add(p);
		}
	}
}

PlaceVector &HashTable::index(const std::string& name) 
{
	return v[hash(name, 100000)];
}

HashTable::~HashTable() 
{
	delete[] v;
}
