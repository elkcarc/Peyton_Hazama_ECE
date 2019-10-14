#define _GLIBCXX_USE_C99 1
#ifndef HASH_H
#define HASH_H

#include<string>
#include<iostream>
#include<fstream>
#include<iomanip> //used to easily print leading zeros in the place numeric codes

struct Place;
struct PlaceVector;
struct HashTable;

struct Place 
{
	int			numeric_code;
	std::string	state_abr;
	std::string	place_name;
	int			population;
	double		area;
	double		lat;
	double		lon;
	int			r_i_code;
	double		int_dist;
	Place();
	Place(int, std::string, std::string, int, double, double, double, int, double);
	void print() const;
};

struct PlaceVector 
{
	Place *		parray;
	size_t		length;
	size_t		absolute_length;
	PlaceVector();
	void add(Place);
	void printStates() const;
	void getState(const std::string&) const;
};

struct HashTable 
{
	PlaceVector * v;
	HashTable(std::string filename);
	PlaceVector &index(const std::string&);
	~HashTable();
};

#endif
