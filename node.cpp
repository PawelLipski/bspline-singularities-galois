#include <iostream>
#include "cube.h"
#include "vector"
#include "node.h"

using namespace std;


Node::Node() : num(-1) { }

Node::Node(const Cube &node_cube, int node_num) :
	cube(node_cube), num(node_num) {
	}

void Node::add_child(Node *child) {
	children.push_back(child);
}

const vector<Node *> &Node::get_children() const {
	return children;
}

const Cube &Node::get_cube() const {
	return cube;
}

int Node::get_num() const {
	return num;
}

void Node::print_num() const {
	cout << get_num() + 1 << " ";
}

