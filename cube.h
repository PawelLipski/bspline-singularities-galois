//
// Created by bj on 04.01.16.
//

#ifndef BSPLINE_SINGULARITIES_GALOIS_CUBE_H
#define BSPLINE_SINGULARITIES_GALOIS_CUBE_H

#include "coord.h"
#include "vector"

using namespace std;

class Cube {

public:

    Cube();

    Cube(int dims);

    Cube(Coord l, Coord r, Coord u, Coord d);

    Cube(const Cube &cube, int n, int l);

    Coord left() const;

    Coord right() const;

    Coord up() const;

    Coord down() const;

    Coord get_bound(int bound_no) const;

    Coord get_from(int dim) const;

    Coord get_middle(int dim) const;

    Coord get_to(int dim) const;

    Coord get_size(int dim) const;

    int get_level() const;

    int get_num() const;

    int get_dim_cnt() const;

    Cube *get_neighbor(int bound_no) const;

    int get_neighbor_count() const;

    vector<int> get_bsplines() const;

    void set_bounds(int dim, Coord from, Coord to);

    void set_neighbor(int bound_no, Cube *cube);

    bool contained_in_box(const Cube &box) const;

    bool empty() const;

    bool is_point_2D() const;

    bool non_empty() const;

    Coord get_overlapping_part(const Cube &other, int dim_no) const;

    bool overlaps_with(const Cube &other) const;

    void print_full() const;

    void print_id() const;

    void print_bounds() const;

    void print_level_id_and_bsplines() const;

    void split(int dim, Coord coord, Cube *first, Cube *second) const;

    void split_halves(int dim, Cube *first, Cube *second) const;

    void scale_up(int factor);

    void spread(int bound_no, int shift);

    void spread(int shift);

    void pump_or_squeeze(int shift);

    void back_up_bounds();

    void restore_bounds();

    vector<int> compute_bspline_support_2D();

    void add_bspline(int bspline_num);

private:

	// Number of dimensions.
	int dim_cnt;
	// Boundaries of the cube.
	vector<Coord> bounds;
	// bounds' backup, for the sake of restoring after tweaks.
	vector<Coord> backed_up_bounds;
	// Neighbors of the cube (must be separately computed).
    vector<Cube*> neighbors;
	// B-splines covering the cube (must be separately computed).
    vector<int> bsplines;
	// Level and id within the level.
    int level, num;
};

#endif //BSPLINE_SINGULARITIES_GALOIS_CUBE_H
