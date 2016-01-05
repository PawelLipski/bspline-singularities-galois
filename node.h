//
// Created by bj on 04.01.16.
//

#ifndef BSPLINE_SINGULARITIES_GALOIS_NODE_H
#define BSPLINE_SINGULARITIES_GALOIS_NODE_H

#include "cube.h"

class Node {
public:

    Node();

    Node(const Cube &node_cube, int node_num);

    void add_child(Node *child);

    const vector<Node *> &get_children() const;

    const Cube &get_cube() const;

    int get_num() const;

    void print_num() const;

private:

    Cube cube;
    int num;
    vector<Node *> children;
};

#endif //BSPLINE_SINGULARITIES_GALOIS_NODE_H
