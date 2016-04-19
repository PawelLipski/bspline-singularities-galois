#include <iostream>
#include <cmath>
//#include "vector"
#include "node.h"
#include "domain.h"
#include "bsplineNonRect.h"
#include <chrono>
#include <thread>


using namespace std;

int CORRECTNESS_ACCURACY = 10;

Domain::Domain(const Cube &box) {
	add_element(box);
	original_box = box;  // preserve for the object lifetime
}


/*** CHECKS ***/

bool Domain::is_middle_element(const Cube &cube, Coord middle) const {
	return (cube.right() == middle || cube.left() == middle) &&
		(cube.up() == middle || cube.down() == middle);
}

bool Domain::is_horizontal_side_element(const Cube &cube, Coord middle) const {
	return (cube.down() == middle) || (cube.up() == middle);
}

bool Domain::is_vertical_side_element(const Cube &cube, Coord middle) const {
	return (cube.right() == middle) || (cube.left() == middle);
}

bool Domain::overlaps_with_any_other(const Cube &that) const {
	for (const auto& other: elements)
		if (&that != &other && that.overlaps_with(other))
			return true;
	return false;
}

/*** SPLIT ELEMENTS ***/

void Domain::split_eight_side_elements_within_box_2D(Cube cube) {
	Coord mid = cube.get_middle(X_DIM);
	vector<Cube> old_elements;
	elements.swap(old_elements);
	for (const auto& e: old_elements) {
		if (e.non_empty() && e.contained_in_box(cube) && !is_middle_element(e, mid)) {
			if (is_horizontal_side_element(e, mid)) {
				Cube el1, el2;
				e.split_halves(Y_DIM, &el1, &el2);
				add_element(el1);
				add_element(el2);
			} else if(is_vertical_side_element(e, mid)) {
				Cube el1, el2;
				e.split_halves(X_DIM, &el1, &el2);
				add_element(el1);
				add_element(el2);
			} else {
				add_element(e);
			}
		} else {
			add_element(e);
		}
	}
}

// Splits each element within the given box into 4 smaller ones.
void Domain::split_elements_within_box_2D(const Cube &box) {
	vector<Cube> old_elements;
	elements.swap(old_elements);
	for (const auto& e: old_elements) {
		if (e.non_empty() && e.contained_in_box(box)) {
			Cube el, er;
			e.split_halves(X_DIM, &el, &er);
			Cube el1, el2, er1, er2;
			el.split_halves(Y_DIM, &el1, &el2);
			er.split_halves(Y_DIM, &er1, &er2);
			add_element(el1);
			add_element(el2);
			add_element(er1);
			add_element(er2);
		} else {
			add_element(e);
		}
	}
}

// Splits each element into 4 smaller ones.
void Domain::split_all_elements_2D() {
	split_elements_within_box_2D(original_box);
}


/*** ADD ELEMENTS ***/

// Inserts `count' of edge elements parallel to the given dimension's axis,
// spanning from one side of the `box' to the other in the given dimension.
// The other dimension is fixed at `coord'.
void Domain::add_edge_2D(int dim, const Cube &box, Coord coord, int count, bool edged_8) {
	Coord from = box.get_from(dim);
	Coord to = box.get_to(dim);
	Coord element_size = (to - from) / count;
	for (int i = 0; i < count; i++) {
		Coord element_from = from + element_size * i;
		Coord element_to = element_from + element_size;

		//needs a flag, only for the new mesh
		if (edged_8) {
			if (i == count / 2 || i == (count / 2 - 1)) {
				Cube e1(2), e2(2);
				Coord mid = (element_from + element_to) / 2;
				e1.set_bounds(dim, element_from, mid);
				e1.set_bounds(dim ^ 1, coord, coord);  // the other dimension
				elements.push_back(e1);
				e2.set_bounds(dim, mid, element_to);
				e2.set_bounds(dim ^ 1, coord, coord);  // the other dimension
				elements.push_back(e2);
				if(dim == X_DIM) {
					add_vertex_2D(mid, coord);
				} else {
					add_vertex_2D(coord, mid);
				}
			} else {
				Cube e(2);
				e.set_bounds(dim, element_from, element_to);
				e.set_bounds(dim ^ 1, coord, coord);  // the other dimension
				elements.push_back(e);
			}
		} else {
			Cube e(2);
			e.set_bounds(dim, element_from, element_to);
			e.set_bounds(dim ^ 1, coord, coord);  // the other dimension
			elements.push_back(e);
		}
	}
}

// For each of the corners of the given box, inserts an infinitely small
// "vertex" element.
void Domain::add_corner_vertices_2D(const Cube &box) {
	add_vertex_2D(box.left(), box.up());
	add_vertex_2D(box.left(), box.down());
	add_vertex_2D(box.right(), box.up());
	add_vertex_2D(box.right(), box.down());
}


/*** PRINT ELEMENTS ***/

void Domain::print_elements_within_box(const Cube &box) const {
	cout << elements.size() << endl;
	for (const auto& e: elements) {
		if (e.contained_in_box(box)) {
			e.print_bounds();
			cout << endl;
		}
	}
}

void Domain::print_elements_level_and_id_within_box(const Node *node) const {
	for (const auto& e: elements) {
		if (e.non_empty() && e.contained_in_box(node->get_cube())) {
			cout << e.get_level() << " " << e.get_id_within_level() << " ";
		}
	}
}

void Domain::print_all_elements() const {
	print_elements_within_box(original_box);
}

void Domain::println_non_empty_elements_count() const {
	cout << count_non_empty_elements() << endl;
}

void Domain::print_tree_nodes_count() const {
	cout << get_tree_nodes().size() << endl;
}

void Domain::print_galois_output() const {
	print_bsplines_line_by_line();
	print_bsplines_per_elements();
	print_elements_per_tree_nodes();
}

void Domain::print_line(Coord x1, Coord y1, Coord x2, Coord y2) {
	cout << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
}

void Domain::print_tabs(int cnt) {
	for (int i = 0; i < cnt; i++)
		cout << "  ";
}


/*** NEIGHBORS ***/

bool Domain::cubes_are_adjacent(const Cube &that, const Cube &other, int bound_no, bool looseened_conds) const {
	int given_dim_no = bound_no >> 1;
	int opposite_bound_no = bound_no ^ 1;
	if (that.get_bound(bound_no) != other.get_bound(opposite_bound_no))
		return false;

	for (int dim_no = 0; dim_no < that.get_dim_cnt(); dim_no++) {
		if (dim_no == given_dim_no)
			continue;

		Coord overlap = that.get_overlapping_part(other, dim_no);
		if (!looseened_conds) {
			if (overlap != that.get_size(dim_no) && overlap != other.get_size(dim_no))
				return false;
		} else {
			if (overlap == 0)
				return false;
		}
	}
	return true;
}

void Domain::print_all_neighbors() const {
	int total_cnt = 0;
	for (const Cube& e: elements)
		total_cnt += e.get_neighbor_count();
	cout << total_cnt << endl;
	for (const Cube& that: elements) {
		for (int bound_no = 0; bound_no < that.get_dim_cnt() * 2; bound_no++) {
			Cube* other = that.get_neighbor(bound_no);
			if (other != nullptr) {
				print_line(
						that.get_middle(X_DIM), that.get_middle(Y_DIM),
						other->get_middle(X_DIM), other->get_middle(Y_DIM)
						);
			}
		}
	}
}

void Domain::compute_neighbors(Cube &that, Coord size) {
	// Check regular neighbors.
	for (auto& other: elements)
		for (int bound_no = 0; bound_no < that.get_dim_cnt() * 2; bound_no++)
			if (cubes_are_adjacent(that, other, bound_no, false))
				that.set_neighbor(bound_no, &other);
	// Check extra neigbors if regular are not found.
	for (int bound_no = 0; bound_no < that.get_dim_cnt() * 2; bound_no++) {
		if(that.get_neighbor(bound_no) == nullptr &&
				that.get_bound(bound_no) != 0 && that.get_bound(bound_no) != size) {
			for (auto& other: elements) {
				if (cubes_are_adjacent(that, other, bound_no, true))
					that.set_neighbor(bound_no, &other);
			}
		}
	}
}

void Domain::compute_all_neighbors(Coord size) {
	for (auto& e: elements)
		compute_neighbors(e, size);
}


/*** TREE ***/

void Domain::tree_process_box_2D(const Cube &box) {
	cut_off_boxes.push_back(box);
}

vector<Cube> Domain::get_cut_off_boxes() const {
	return cut_off_boxes;
}

int Domain::count_elements_within_box(const Cube &cube) const {
	int count = 0;
	for (const auto& e: elements)
		if (e.non_empty() && e.contained_in_box(cube))
			count++;
	return count;
}

Node *Domain::add_tree_node(Cube cube, Node *parent) {
	Node* node = new Node(cube, tree_node_id++);
	if (parent) {
		parent->add_child(node);
	}
	tree_nodes.push_back(node);
	return node;
}

void Domain::tree_process_cut_off_box(int dim, Node *node, bool toggle_dim) {
	if (count_elements_within_box(node->get_cube()) == 1)
		return;

	Cube cut_off_cube = node->get_cube();
	Cube first_half, second_half;
	cut_off_cube.split_halves(dim, &first_half, &second_half);

	Node* first_half_node = this->add_tree_node(first_half, node);
	Node* second_half_node = this->add_tree_node(second_half, node);

	if (toggle_dim)
		dim ^= 1;

	tree_process_cut_off_box(dim, first_half_node, toggle_dim);
	tree_process_cut_off_box(dim, second_half_node, toggle_dim);
}

const vector<Node *> &Domain::get_tree_nodes() const {
	return tree_nodes;
}

void Domain::print_elements_per_tree_nodes() const {
	print_tree_nodes_count();
	for (const Node* node: get_tree_nodes()) {
		node->print_num();
		print_elements_count_within_node(node);
		print_elements_level_and_id_within_box(node);
		print_node_children(node);
	}
}

void Domain::print_tree_size() const {
	cout << get_cut_off_boxes().size() << endl;
}

void Domain::print_tree_for_draw() const {
	print_tree_size();
	for (const Cube& box: get_cut_off_boxes()) {
		box.print_full();
	}
}

void Domain::print_node_children(const Node *node) const {
	for (const Node* n: node->get_children()) {
		cout << n->get_num() + 1 << " ";
	}
	cout << endl;
}

void Domain::print_elements_count_within_node(const Node *node) const {
	cout << count_elements_within_box(node->get_cube()) << " ";
}


/*** BOUNDARY TWEAKING ***/

void Domain::tweak_bounds() {
	for (auto& e: elements)
		e.back_up_bounds();
	original_box.back_up_bounds();

	// Scale up all elements.
	for (auto& e: elements)
		e.scale_up(8);
	original_box.scale_up(8);

	// Squeeze non-empty dims, pump up empty dims.
	for (auto& e: elements)
		e.pump_or_squeeze(2);

	// Try pump back non-empty dims (only if they won't overlap with now pumped-up empty dims).
	for (auto& e: elements) {
		for (int bound_no = 0; bound_no < 2 * e.get_dim_cnt(); bound_no++) {
			if (e.get_size(bound_no / 2) == 2)
				continue;  // skip empty dims
			e.spread(bound_no, 2);
			if (overlaps_with_any_other(e)) {
				// Roll back the pumping if it interferes with any other element.
				e.spread(bound_no, -2);
			}
		}
	}
	original_box.spread(2);
}

void Domain::untweak_bounds() {
	for (auto& e: elements)
		e.restore_bounds();
	original_box.restore_bounds();
}


/*** B-SPLINES ***/

void Domain::compute_bsplines_supports(MeshType type, int order) {
	for (auto& e: elements) {
		compute_bspline_support(type, order, e, e.get_num());
	}
	check_bsplines_correctness(CORRECTNESS_ACCURACY);
	//cout << "bsplines count: " << bsplines2D.size() << endl;
}

void Domain::compute_bspline_support(MeshType type, int order, Cube &e, int original_bspline_num) {
	const vector<Coord> &support_bounds = e.compute_bspline_support_2D();
	const Cube &support_cube = Cube(support_bounds[0], support_bounds[1], support_bounds[2], support_bounds[3]);


//	if (e.is_point_2D() && support_cube.get_size(X_DIM) > 4) {
//		//non-rect-support detection must be implemented here
//		Cube not_defined_cube = compute_not_defined_cube(e, support_cube);
//		cout << "not_defined_cube:";
//		not_defined_cube.print_bounds();
//		cout << endl;
//		//Bspline2DNonRect
//	} else {
//
	const vector<double> &x_knots = e.get_dim_knots(support_cube, X_DIM);
	//	cout << "x_knots: ";
	//	for (int i = 0; i < x_knots.size(); ++i) {
	//		cout << x_knots[i] << " ";
	//	}
	//	cout << endl;
	const vector<double> &y_knots = e.get_dim_knots(support_cube, Y_DIM);
	const Bspline2D &bspline = Bspline2D(x_knots, y_knots, 1.0);
	//
	//	cout << "y_knots: ";
	//	for (int i = 0; i < y_knots.size(); ++i) {
	//		cout << y_knots[i] << " ";
	//	}
	//	cout << endl;
	//
	//	cout << "bspline support: ";
	//	bspline.get_support().print_bounds();
	//	cout << endl;
	add_bspline2D(bspline);
//	}


	for (auto &support_candidate: elements) {
			if (support_candidate.non_empty() && support_candidate.contained_in_box(support_cube)) {
				if (type == EDGED_4 && e.is_point_2D()) {
					Coord min_el_size = e.get_neighbor(0)->get_size(0) / 2;
					if (min_el_size > 1 && support_candidate.get_size(0) < min_el_size) {
						continue;
					}
				}
				if (!support_candidate.is_bspline_duplicated(original_bspline_num)) {
					support_candidate.add_bspline(original_bspline_num);
				}
				if (order > 2) {
					compute_bspline_support(type, order - 1, support_candidate, original_bspline_num);
				}
			}
		}
}

Cube Domain::compute_not_defined_cube(const Cube &e, const Cube &support_cube) const {
	Coord middle = original_box.get_size(X_DIM) / 2;
//	cout << "middle: " << middle << endl;
//	cout << "source el: ";
//	e.print_bounds();
//	cout << endl;
//	cout << "support bounds: ";
//	support_cube.print_bounds();
//	cout << endl;

	//{l, u, r, d}
	vector<Coord> not_defined_vector;

	if (e.get_bound(0) < middle) {
		//cout << "left hand side" << endl;
		if (e.get_bound(2) < middle) {
			//cout << "up" << endl;
			not_defined_vector = {support_cube.get_middle(X_DIM),
								  support_cube.get_middle(Y_DIM),
								  support_cube.get_bound(1),
								  support_cube.get_bound(3)};
		} else {
			//cout << "down" << endl;
			not_defined_vector = {support_cube.get_middle(X_DIM),
								  support_cube.get_bound(2),
								  support_cube.get_bound(1),
								  support_cube.get_middle(Y_DIM)};
		}
	} else {
		//cout << "right hand side" << endl;
		if (e.get_bound(2) < middle) {
			//cout << "up" << endl;
			not_defined_vector = {support_cube.get_bound(0),
								  support_cube.get_middle(Y_DIM),
								  support_cube.get_middle(X_DIM),
								  support_cube.get_bound(3)};
		} else {
			//cout << "down" << endl;
			not_defined_vector = {support_cube.get_bound(0),
								  support_cube.get_bound(2),
								  support_cube.get_middle(X_DIM),
								  support_cube.get_middle(Y_DIM)};
		}
	}

	Cube not_defined_cube(not_defined_vector[0], not_defined_vector[2], not_defined_vector[1], not_defined_vector[3]);
	return not_defined_cube;
}

void Domain::print_support_for_each_bspline() const {
	vector<vector<Coord>> supports(elements.size());
	for (const auto& e: elements) {
		for (Coord bspline: e.get_bsplines()) {
			supports[bspline].push_back(e.get_num());
		}
	}
	cout << elements.size() << endl;
	for (unsigned i = 0; i < elements.size(); i++) {
		const auto& e = elements[i];
		cout << e.get_middle(X_DIM) << " " << e.get_middle(Y_DIM) << " ";

		const auto& support = supports[i];
		cout << support.size() << " ";
		for (auto& s: support)
			cout << s << " ";
		cout << endl;
	}
}

void Domain::print_knots_for_each_bspline() const {
	for (const Bspline2D& bspline: bsplines2D) {
		for (auto coord: bspline.get_x_knots())
			cout << coord << " ";
		for (auto coord: bspline.get_y_knots())
			cout << coord << " ";
		cout << endl;
	}
}

void Domain::print_bsplines_per_elements() const {
	println_non_empty_elements_count();
	for(auto& e : elements)
		if (e.non_empty()) {
			e.print_level_id_and_bsplines(e.get_id_within_level());
		}
}

void Domain::print_bsplines_line_by_line() const {
	cout << elements.size() << endl;
	for (const auto& e: elements)
		cout << e.get_num() + 1 << " " << 1 << endl;
}


/*** UTILS ***/

int Domain::count_non_empty_elements() const {
	int count = 0;
	for (const auto& e: elements)
		if (e.non_empty())
			count++;
	return count;
}

int Domain::compute_level(const Cube &cube) const {
	Coord size = max(cube.get_size(0), cube.get_size(1));
	return (int) log2((original_box.get_size(0) / size)) - 1;
}

void Domain::enumerate_all_elements() {
	vector<Cube> old_elements;
	elements.swap(old_elements);
	int i = 0;
	for (const auto& e: old_elements) {
		if (e.non_empty()) {
			int level = compute_level(e);
			int id = get_e_num_per_level_and_inc(level);
			elements.push_back(Cube(e, i++, level, id, -1));
		} else {
			elements.push_back(Cube(e, i++, -1, -1, -1));
		}
	}
}

void Domain::add_vertex_2D(Coord x, Coord y) {
	add_element_2D(x, x, y, y);
}

void Domain::add_element_2D(Coord left, Coord right, Coord up, Coord down) {
	add_element(Cube(left, right, up, down));
}

void Domain::add_element(const Cube &e) {
	elements.push_back(e);
}


void Domain::allocate_elements_count_by_level_vector(int depth) {
	elements_count_by_level.resize(depth + 1);
}

int Domain::get_e_num_per_level_and_inc(int level) const {
	return ++elements_count_by_level[level];
}

void Domain::add_bspline2D(const Bspline2D &bspline2D) {
	bsplines2D.push_back(bspline2D);
}

void Domain::check_bsplines_correctness(int accuracy) {
	for (const auto &e: elements) {
		double x, y;
		double x_step = (double) e.get_size(X_DIM) / accuracy;
		double y_step = (double) e.get_size(Y_DIM) / accuracy;
		//e.print_bounds();
		//cout << endl;
		//cout << "x_step: " << x_step << endl;
		//cout << "y_step: " << y_step << endl;
		for (int i = 0; i <= accuracy; ++i) {
			for (int j = 0; j <= accuracy; ++j) {
				x = e.get_bound(0) + i * x_step;
				y = e.get_bound(2) + j * y_step;
				double bsplines_sum = 0;
				for (const auto &bspline: bsplines2D) {
					if (e.contained_in_box(bspline.get_support())) {
						bsplines_sum += bspline.apply(x, y);
						//cout << "adding for (x,y) = (" << x << "," << y <<"): " << bspline.apply(x,y) <<  endl;
					}
				}
				if (bsplines_sum <= 0.9 || bsplines_sum >= 1.1) {
					//cout << "ERROR: sum for (x,y) = (" << x << "," << y << "): " << " is: " << bsplines_sum << endl;
				}
			}
		}
	}
}
