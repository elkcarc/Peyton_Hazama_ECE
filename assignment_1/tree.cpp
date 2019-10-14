#include "tree.h"

node::node()
{
	left = 0;
	right = 0;
	count = 0;
}

node::node(std::string &s)
{
	data = s;
	left = 0;
	right = 0;
	count = 0;
}

node::~node(){ }

int tree::_compareStringIgnoreCase(node * a, std::string &s)
{
	int length;
	std::string x = a->data;
	std::string y = s;
	for (int i = 0; i < x.length(); i++)
	{
		if (x[i] < 97)
		{
			x[i] = x[i] + 32;
		}
	}
	for (int i = 0; i < y.length(); i++)
	{
		if (y[i] < 97)
		{
			y[i] = y[i] + 32;
		}
	}
	if (a->data.length() > s.length())
	{
		length = s.length() + 1;
	}
	else
	{
		length = a->data.length() + 1;
	}
	for (int i = 0; i < length; i++)
	{
		if (x[i] < y[i])
		{
			return 1;
		}
		else if (x[i] > y[i])
		{
			return -1;
		}
	}
	return 0;
}

node * tree::_addString(node * r, std::string &s)
{
	if (r == 0)
	{
		node *n = new node(s);
		r = n;
		_count++;
		r->count++;
		return n;
	}
	else
	{
		if (1 == _compareStringIgnoreCase(r, s))
		{
			r->right = _addString(r->right, s);
			//std::cout << "right" << std::endl;
		}
		else if (-1 == _compareStringIgnoreCase(r, s))
		{
			r->left = _addString(r->left, s);
			//std::cout << "left" << std::endl;
		}
		else if (0 == _compareStringIgnoreCase(r, s))
		{
			r->count++;
		}
		return r;
	}
}

node * tree::_printTree(node * r)
{
	if (r != 0)
	{
		std::cout << r->data << " seen " << r->count << " times." << std::endl;
	}
	if (r->left != 0)
	{
		r->left = _printTree(r->left);
	}
	if (r->right != 0)
	{
		r->right = _printTree(r->right);
	}
	return r;
}

node * tree::_searchTree(node * r, std::string & s, bool f)
{
	if (r != 0 && 0 == _compareStringIgnoreCase(r, s))
	{
		if (r->count == 1)
		{
			std::cout << "Yes, '" << r->data << "' seen " << r->count << " time." << std::endl;
		}
		else
		{
			std::cout << "Yes, '" << r->data << "' seen " << r->count << " times." << std::endl;
		}
		f = true;
	}
	if (r->left != 0 && -1 == _compareStringIgnoreCase(r, s))
	{
		r->left = _searchTree(r->left, s, f);
		return r;
	}
	if (r->right != 0 && 1 == _compareStringIgnoreCase(r, s))
	{
		r->right = _searchTree(r->right, s, f);
		return r;
	}
	if (!f)
	{
		std::cout << "No, '" << s << "' was not seen." << std::endl;
	}
	return r;
}

int tree::addString(std::string & s)
{
	_root = _addString(_root, s);
	return _count;
}

tree::tree()
{
	_root = 0;
	_count = 0;
}

tree::~tree()
{
	clearPostOrder(_root);
}

void tree::clear()
{
	clearPostOrder(_root);
}

void tree::clearPostOrder(node *r) 
{
	if (r != 0) {
		clearPostOrder(r->left); // delete the left subtree
		clearPostOrder(r->right); // delete the right subtree
		delete r; // delete the node
	}
}

void tree::printTree()
{
	_printTree(_root);
}

void tree::printNode(node * r)
{
	std::cout << r->data << std::endl;
}

void tree::printRoot()
{
	std::cout << _root->data << std::endl;
}

void tree::searchTree(std::string & s)
{
	_searchTree(_root, s, false);
}
