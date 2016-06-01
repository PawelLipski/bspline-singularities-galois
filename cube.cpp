#include "cube.h"
#include "coord.h"
#include <vector>
#include <iostream>

using namespace std;

// A general hyper-cube for any number of dim_cnt.

Cube::Cube() {}

Cube::Cube(int dims): dim_cnt(dims) {
	bounds.resize(dims * 2);
	neighbors.resize(dims * 2);
}

Cube::Cube(Coord l, Coord r, Coord u, Coord d): dim_cnt(2) {
	bounds = { l, r, u, d };
	neighbors.resize(4);
}

Cube::Cube(const Cube &cube, int n, int l, int id, int flag) :
	dim_cnt(cube.dim_cnt), bounds(cube.bounds), neighbors(cube.neighbors), level(l), num(n), id_within_level(id) {
	}


/*** GETTERS ***/

Coord Cube::get_bound(int bound_no) const {
	return bounds[bound_no];
}

Coord Cube::get_from(int dim) const {
	return bounds[2*dim];
}

Coord Cube::get_middle(int dim) const {
	return (get_from(dim) + get_to(dim)) / 2;
}

Coord Cube::get_to(int dim) const {
	return bounds[2*dim+1];
}

Coord Cube::get_size(int dim) const {
	return get_to(dim) - get_from(dim);
}

Coord Cube::left()  const { return bounds[0]; }
Coord Cube::right() const { return bounds[1]; }
Coord Cube::up()    const { return bounds[2]; }
Coord Cube::down()  const { return bounds[3]; }

int Cube::get_dim_cnt() const {
	return dim_cnt;
}

int Cube::get_num() const {
	return num;
}

int Cube::get_id_within_level() const {
	return id_within_level;
}

int Cube::get_level() const {
	return level;
}

Cube* Cube::get_neighbor(int bound_no) const {
	return neighbors[bound_no];
}

int Cube::get_neighbor_count() const {
	int cnt = 0;
	for (int bound_no = 0; bound_no < 2 * dim_cnt; bound_no++)
		if (neighbors[bound_no] != nullptr)
			cnt++;
	return cnt;
}

vector<int> Cube::get_bsplines() const {
	return bsplines;
}


/*** SETTERS ***/

void Cube::set_bounds(int dim, Coord from, Coord to) {
	bounds[dim * 2] = from;
	bounds[dim * 2 + 1] = to;
}

void Cube::set_neighbor(int bound_no, Cube* cube) {
	neighbors[bound_no] = cube;
}


/*** CHECKS ***/

// Whether this cube is fully contained within the given box.
bool Cube::contained_in_box(const Cube& box) const {
	for (int i = 0; i < dim_cnt; i++)
		if (!(box.get_from(i) <= get_from(i) && get_to(i) <= box.get_to(i)))
			return false;
	return true;
}

bool Cube::empty() const {
	return !non_empty();
}

bool Cube::is_point_2D() const {
	return get_size(X_DIM) + get_size(Y_DIM) == 0;
}

bool Cube::non_empty() const {
	for (int i = 0; i < dim_cnt; i++)
		if (get_size(i) == 0)
			return false;
	return true;
}


/*** OVERLAPPING & ENCLOSING ***/

Coord Cube::get_overlapping_part(const Cube& other, int dim_no) const {
	Coord from = max(get_from(dim_no), other.get_from(dim_no));
	Coord to = min(get_to(dim_no), other.get_to(dim_no));
	return max(to - from, 0L);
}

bool Cube::overlaps_with(const Cube& other) const {
	for (int dim_no = 0; dim_no < dim_cnt; dim_no++) {
		if (get_to(dim_no) <= other.get_from(dim_no))
			return false;
		if (other.get_to(dim_no) <= get_from(dim_no))
			return false;
	}
	return true;
}

Cube Cube::get_cube_enclosing_both(const Cube& other) const {
	Cube result(get_dim_cnt());
	for (int dim_no = 0; dim_no < get_dim_cnt(); dim_no++) {
		Coord from = min(get_from(dim_no), other.get_from(dim_no));
		Coord to = max(get_to(dim_no), other.get_to(dim_no));
		result.set_bounds(dim_no, from, to);
	}
	return result;
}


/*** PRINTING ***/

void Cube::print_full() const {
	print_bounds();
	print_id();
	cout << endl;
}

void Cube::print_id() const {
	cout << get_num()+1 << " " << get_level() << " ";
}

void Cube::print_bounds() const {
	for (int i = 0; i < dim_cnt * 2; i++)
		cout << bounds[i] << " ";
}

void Cube::print_level_id_and_bsplines(int e_num) const {
	cout << get_level() << " ";
	cout << e_num << " ";
	cout << bsplines.size();
	for (int bspline: bsplines)
		cout << " " << bspline + 1;
	cout << endl;
}


/*** SPLITTING ***/

// Splits this cube into two cubes, along the given dimension at the given
// coordinate, and puts the new cubes into the given pointers.
void Cube::split(int dim, Coord coord, Cube* first, Cube* second) const {
	*first = *this;
	*second = *this;
	first->set_bounds(dim, get_from(dim), coord);
	second->set_bounds(dim, coord, get_to(dim));
}

void Cube::split_halves(int dim, Cube* first, Cube* second) const {
	split(dim, get_middle(dim), first, second);
}

void Cube::split_thirds(int dim, Cube *first, Cube *second, Cube *third) const {
	//to be implented
	//split(dim, get_middle(dim), first, second);
}


/*** TWEAKING ***/

void Cube::scale_up(int factor) {
	for (int bound_no = 0; bound_no < 2 * dim_cnt; bound_no++)
		bounds[bound_no] *= factor;		
}

void Cube::spread(int bound_no, int shift) {
	if (bound_no % 2 == 0)
		bounds[bound_no] -= shift; // from
	else
		bounds[bound_no] += shift; // to
}

void Cube::spread(int shift) {
	for (int bound_no = 0; bound_no < 2 * dim_cnt; bound_no++)
		spread(bound_no, shift);
}

void Cube::pump_or_squeeze(int shift) {
	for (int dim_no = 0; dim_no < dim_cnt; dim_no++) {
		if (get_size(dim_no) == 0) {
			spread(2*dim_no,   +shift);
			spread(2*dim_no+1, +shift);
		} else {
			spread(2*dim_no,   -shift);
			spread(2*dim_no+1, -shift);
		}
	}
}

void Cube::back_up_bounds() {
	backed_up_bounds = bounds;
}

void Cube::restore_bounds() {
	bounds = backed_up_bounds;
}


/*** B-SPLINES ***/

vector<Coord> Cube::compute_bspline_support_2D() {
	vector<Coord> support_bounds;
	support_bounds.resize(dim_cnt * 2);
	for(int i = 0; i < dim_cnt * 2; ++i) {
		if (neighbors[i]) {
			support_bounds[i] = neighbors[i]->get_bound(i);
		} else {
			support_bounds[i] = bounds[i];
		}
	}
	return support_bounds;
};

bool Cube::is_bspline_duplicated(int bspline_num) {
	for (int curr_bspline: bsplines) {
		if (curr_bspline == bspline_num) {
			return true;
		}
	}
	return false;
}

void Cube::add_bspline(int bspline_num) {
	bsplines.push_back(bspline_num);
}

vector<double> Cube::get_dim_knots(const Cube &support_cube, int dim) {
	int from = 2 * dim;
	int to = 2 * dim + 1;
	vector<double> knots;
	knots.resize(dim_cnt * 2);
	//middle elements of knots are always bounds of bspline source element
	knots[1] = get_bound(from);
	knots[2] = get_bound(to);
	//1st and last elements of knots are always bspline support boundaries
	knots[0] = support_cube.get_bound(from);
	knots[3] = support_cube.get_bound(to);
	return knots;
}
