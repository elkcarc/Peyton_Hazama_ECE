#ifndef DICTIONARYTREE_H
#define DICTIONARYTREE_H
#include <iostream>
#include <string>

struct node
{
	std::string data;
	node* left;
	node* right;
	int count;

	node();
	node(std::string &s);
	~node();
};


class tree
{
	node* _root;
	int _count;
	int _compareStringIgnoreCase(node *a, std::string &s);
	node* _addString(node *r, std::string &s);
	node* tree::_printTree(node * r);
	node* tree::_searchTree(node * r, std::string &s, bool f);

public:

	tree();
	~tree();

	void clear();
	void clearPostOrder(node * r);
	int tree::addString(std::string &s);
	void printTree();
	void tree::printNode(node * r);
	void tree::printRoot();
	void tree::searchTree(std::string &s);
};

#endif

