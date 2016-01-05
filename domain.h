//
// Created by bj on 05.01.16.
//

#ifndef BSPLINE_SINGULARITIES_GALOIS_DOMAIN_H
#define BSPLINE_SINGULARITIES_GALOIS_DOMAIN_H

#include "node.h"

class Domain {
public:
    Domain(const Cube &box);

    bool is_middle_element(const Cube &cube, Coord middle) const;

    bool is_horizontal_side_element(const Cube &cube, Coord middle) const;

    bool is_vertical_side_element(const Cube &cube, Coord middle) const;

    bool overlaps_with_any_other(const Cube &that) const;

    void split_eight_side_elements_within_box_2D(Cube cube);

    void split_elements_within_box_2D(const Cube &box);

    void split_all_elements_2D();

    void add_edge_2D(int dim, const Cube &box, Coord coord, int count, bool edged_8);

    void add_corner_vertices_2D(const Cube &box);

    void print_elements_within_box(const Cube &box) const;

    void print_elements_level_and_id_within_box(const Node *node) const;

    void print_all_elements() const;

    void println_non_empty_elements_count() const;

    void print_el_tree_nodes_count() const;

    void print_galois_output() const;

    static void print_line(Coord x1, Coord y1, Coord x2, Coord y2);

    static void print_tabs(int cnt);

    bool cubes_are_adjacent(const Cube &that, const Cube &other, int bound_no, bool looseened_conds) const;

    void print_all_neighbors() const;

    void compute_neighbors(Cube &that, int size);

    void compute_all_neighbors(int size);

    void tree_process_box_2D(const Cube &box);

    vector<Cube> get_cut_off_boxes() const;

    int count_elements_within_box(const Cube &cube) const;

    Node *add_el_tree_element(Cube cube, Node *parent);

    void try_to_tree_process(int dim, Node *node, bool toggle_dim);

    void tree_process_cut_off_box(int dim, Node *node, bool toggle_dim);

    const vector<Node *> &get_el_tree_nodes() const;

    void print_elements_per_el_tree_nodes() const;

    void print_el_tree_size() const;

    void print_el_tree_for_draw() const;

    void print_node_children(const Node *node) const;

    void print_elements_count_within_node(const Node *node) const;

    void tweak_bounds();

    void untweak_bounds();

    void compute_bsplines_supports();

    void print_support_for_each_bspline() const;

    void print_bsplines_per_elements() const;

    void print_bsplines_line_by_line() const;

    int count_non_empty_elements() const;

    int compute_level(const Cube &cube) const;

    void enumerate_all_elements();

private:

    void add_vertex_2D(Coord x, Coord y);

    void add_element_2D(Coord left, Coord right, Coord up, Coord down);

    void add_element(const Cube &e);

    Cube original_box;
    vector<Cube> elements;
    vector<Cube> cut_off_boxes;
    vector<Node *> el_tree_nodes;
    int el_tree_node_id = 0;
};

#endif //BSPLINE_SINGULARITIES_GALOIS_DOMAIN_H
