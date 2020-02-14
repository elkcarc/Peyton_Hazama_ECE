#ifndef MAP_H
#define MAP_H
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <cmath>

struct connections
{
  std::string name;
  std::string code;
  int inter_a;
  int inter_b;
  double road_len;

  connections();
  connections(std::string name, std::string code, int inter_a, int inter_b, double road_len);
  void print() const;
  void print(int code) const;
  void printdist() const;
};

struct i_node
{
  int line;
  double lon;
  double lat;
  double dist;
  std::string state;
  std::string name;
  std::vector<connections*> paths;
  i_node* parent;
  double fscore;
  double gscore;

  i_node();
  i_node(int line, double lon, double lat, double dist, std::string state, std::string name, std::vector<connections> &all_paths);
  void getConnections(std::vector<connections*> &paths, std::vector<connections> &all_paths, int line);
  void print() const;
  void printnodist() const;
  void printpaths() const;
  void setf(int f);
  void setg(int g);
};


class map
{
	std::vector<i_node> i_map;
	std::vector<connections> all_paths;

public:
	map();
	map(std::string intersectionFile, std::string connectionsFile);
	~map();
	void create_map(std::string intersectionFile, std::string connectionsFile);
  i_node getnode(int code);
	void explore_map(int code);
  std::vector<connections*> getNeighbors(std::vector<i_node> i_map, int code);
  double heuristic(int start, int end);
  i_node * AStar(int start, int goal);
  int shortestPath(int s, int e);
};

#endif
