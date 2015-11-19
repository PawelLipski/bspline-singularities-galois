#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

typedef int Coord;

enum {
	X_DIM = 0,
	Y_DIM = 1
};

// A general hyper-cube for any number of dimensions.
class Cube {

	public:

	Cube(){}

	Cube(int dims): dimensions(dims) {
		limits.resize(dims * 2);
        neighbours.resize(dims * 2);
	}

	Cube(Coord l, Coord r, Coord u, Coord d): dimensions(2) {
		limits = { l, r, u, d };
		neighbours.resize(4);
	}

    Cube(const Cube& cube, int n, int l):
        dimensions(cube.dimensions), limits(cube.limits), num(n) , lvl(l), neighbours(cube.neighbours){
	}

    Cube(const Cube &cube, const vector<const Cube *> &neighbours):
            dimensions(cube.dimensions), limits(cube.limits), num(cube.num) , lvl(cube.lvl), neighbours(neighbours){
    }


    // Whether this cube is fully contained within the given box.
	bool contained_in_box(const Cube& box) const {
		for (int i = 0; i < dimensions; i++)
			if (!(box.get_from(i) <= get_from(i) && get_to(i) <= box.get_to(i)))
				return false;
		return true;
	}

	bool empty() const {
		return !non_empty();
	}

	inline Coord get_from(int dimension) const {
		return limits[2*dimension];
	}

	Coord get_middle(int dimension) const {
		return (get_from(dimension) + get_to(dimension)) / 2;
	}

	Coord get_size(int dimension) const {
		return get_to(dimension) - get_from(dimension);
	}

	bool is_point() const {
		return get_size(X_DIM) + get_size(Y_DIM) == 0;
	}

	inline Coord get_to(int dimension) const {
		return limits[2*dimension+1];
	}

	bool non_empty() const {
		for (int i = 0; i < dimensions; i++)
			if (get_size(i) == 0)
				return false;
		return true;
	}

	void print_full() const {
		print_limits();
		print_id();
		cout << endl;
	}

	void print_id() const {
		cout << num << " " << get_lvl() << " ";
	}

	void print_limits() const {
		for (int i = 0; i < dimensions * 2; i++)
			cout << limits[i] << " ";
	}

	void set_limits(int dimension, Coord from, Coord to) {
		limits[dimension * 2] = from;
		limits[dimension * 2 + 1] = to;
	}

	// Splits this cube into two cubes, along the given dimension at the given
	// coordinate, and puts the new cubes into the given pointers.
	void split(int dimension, Coord coord, Cube* first, Cube* second) const {
		*first = *this;
		*second = *this;
		first->set_limits(dimension, get_from(dimension), coord);
		second->set_limits(dimension, coord, get_to(dimension));
	}

	void split_halves(int dimension, Cube* first, Cube* second) const {
		split(dimension, get_middle(dimension), first, second);
	}

	Coord left()  const { return limits[0]; }
	Coord right() const { return limits[1]; }
	Coord up()    const { return limits[2]; }
	Coord down()  const { return limits[3]; }

    Coord get_coord(int dim) const {
        return limits[dim];
    }

	void set_neighbour(int dimension, const Cube * cube) {
		neighbours[dimension] = cube;
	}

	private:

	// Number of dimensions.
	int dimensions;
	vector<Coord> limits;
    vector<const Cube *> neighbours;
	


public:
    int get_dimensions() const {
        return dimensions;
    }

    int get_num() const {
        return num;
    }

    int get_lvl() const {
        return lvl;
    }
private:
    int num;

    int lvl;
};

class Node{
public:
    Node(): num(-1){ }

    Node(const Cube& node_cube, int node_num): num(node_num) {
        cube = node_cube;
    }

	const Cube &get_cube() const {
		return cube;
	}

	int get_num() const {
		return num;
	}

	void add_child(Node *child) {
		children.push_back(child);
	}

	const vector<Node *> &get_children() const {
		return children;
	}

private:
	int num;
	Cube cube;
	vector<Node*> children;
};

class Domain {
	public:

	Domain(const Cube& box, int depth) {
		add_element(box);
		original_box = box;  // preserve for the object lifetime

	}

	// Splits each element within the given box into 4 smaller ones.
	void split_elements_within_box_2D(const Cube& box) {
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
	void split_all_elements_2D() {
		split_elements_within_box_2D(original_box);
	}

	bool is_middle_element(const Cube &cube, Coord middle) {
		return (cube.right() == middle || cube.left() == middle) &&
			   (cube.up() == middle || cube.down() == middle);
	}

	bool is_horizontal_side_element(const Cube &cube, Coord middle) {
		return (cube.down() == middle) || (cube.up() == middle);
	}

	bool is_vertical_side_element(const Cube &cube, Coord middle) {
		return (cube.right() == middle) || (cube.left() == middle);
	}

	void split_eight_side_elements_within_box_2D(Cube cube) {
		Coord size = cube.get_size(X_DIM);
		Coord mid = cube.get_middle(X_DIM);
		vector<Cube> old_elements;
		elements.swap(old_elements);
		for (const auto& e: old_elements) {
			if (e.non_empty() && e.contained_in_box(cube) && !is_middle_element(e, mid)){
				if(is_horizontal_side_element(e, mid)){
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

	// Inserts `count' of edge elements parallel to the given dimension's axis,
	// spanning from one side of the `box' to the other in the given dimension.
	// The other dimension is fixed at `coord'.
	void add_edge_2D(int dimension, const Cube& box, Coord coord, int count, bool new_net) {
		int from = box.get_from(dimension);
		int to = box.get_to(dimension);
		Coord element_size = (to - from) / count;
		for (int i = 0; i < count; i++) {
			Coord element_from = from + element_size * i;
			Coord element_to = element_from + element_size;

			//needs a flag, only for the new net
			if(new_net){
				if (i == count / 2 || i == (count / 2 - 1)){
					Cube e1(2), e2(2);
					Coord mid = (element_from + element_to) / 2;
					e1.set_limits(dimension, element_from, mid);
					e1.set_limits(dimension ^ 1, coord, coord);  // the other dimension
					elements.push_back(e1);
					e2.set_limits(dimension, mid, element_to);
					e2.set_limits(dimension ^ 1, coord, coord);  // the other dimension
					elements.push_back(e2);
					if(dimension == X_DIM){
						add_vertex_2D(mid, coord);
					} else {
						add_vertex_2D(coord, mid);
					}
				} else {
					Cube e(2);
					e.set_limits(dimension, element_from, element_to);
					e.set_limits(dimension ^ 1, coord, coord);  // the other dimension
					elements.push_back(e);
				}
			} else {
				Cube e(2);
				e.set_limits(dimension, element_from, element_to);
				e.set_limits(dimension ^ 1, coord, coord);  // the other dimension
				elements.push_back(e);
			}
		}
	}

	// For each of the corners of the given box, inserts an infinitely small
	// "vertex" element.
	void add_corner_vertices_2D(const Cube& box) {
		add_vertex_2D(box.left(), box.up());
		add_vertex_2D(box.left(), box.down());
		add_vertex_2D(box.right(), box.up());
		add_vertex_2D(box.right(), box.down());
	}

	void print_elements_within_box(const Cube& box, bool require_non_empty, bool with_id) const {
		cout << elements.size() << endl;
		for (const auto& e: elements) {
			if (require_non_empty && e.empty())
				continue;
			if (!e.contained_in_box(box))
				continue;

			e.print_limits();
			if (with_id)
				e.print_id();
			cout << endl;
		}
	}

	void print_el_lvl_id_within_box(const Cube& box){
		for (const auto& e: elements) {
			if(e.non_empty() && e.contained_in_box(box)){
				cout << e.get_lvl() << " " << e.get_num() << " ";
			}
		}
	}

	void print_all_elements(bool require_non_empty, bool with_id) const {
		print_elements_within_box(original_box, require_non_empty, with_id);
	}

    int compute_lvl(const Cube &cube) {
        int size = cube.get_size(0);
        return (int) log2(size) + 1;
    }

    void enumerate_all_elements() {
        vector<Cube> old_elements;
        elements.swap(old_elements);
        int i = 0;
        for (const auto& e: old_elements) {
            if (e.non_empty()){
                elements.push_back(Cube(e, i++, compute_lvl(e)));
            } else {
                elements.push_back(Cube(e, -1, -1));
            }
        }
    }


    bool cubes_are_adjacent(const Cube &cube, const Cube &potential_neighbour, int dim/*0..3*/) const {
        int opposite_acc_dim = dim % 2 == 0 ? dim + 1 : dim - 1;
        bool are_adjacent = false;
		if(cube.get_coord(dim) == potential_neighbour.get_coord(opposite_acc_dim)){
			for (int i = 0; i < 4; ++i){
				if(i != dim && i != opposite_acc_dim){
					if (cube.get_coord(i) == potential_neighbour.get_coord(i)){
						are_adjacent = true;
					}
				}
			}
        }
		return are_adjacent;
    }


    vector<const Cube *> find_neighbours(const Cube &cube, Coord size) {
        vector<const  Cube *> neighbours;
        neighbours.resize(cube.get_dimensions() * 2);
        for(int dim = 0; dim < cube.get_dimensions() * cube.get_dimensions(); dim++){
            if(cube.get_coord(dim) == 0 || cube.get_coord(dim) == size){
                neighbours[dim] = NULL; // cube doesn't have neighbours on this dimension
            } else {
                bool has_empty_neighbour = false;
                for(const auto& e: elements){
                    if (e.get_num() != cube.get_num()){
                        if (cube.is_point()){ // infinite small vertex
							if(!e.is_point()){
								if(cubes_are_adjacent(cube, e, dim)){
									neighbours[dim] = &e;
								}
							}
						} else if (cube.empty()){ // infinite thin, but long element
							if(e.empty()){
								//??
							}
						} else { //regular cube
							if (!e.is_point()){
								if(e.empty()){
									if(cubes_are_adjacent(cube, e, dim)){
										int general_dim = 0;
										if (dim > 1) general_dim = 1;
										if (cube.get_size(general_dim) <= e.get_size(general_dim)){
											neighbours[dim] = &e;
											has_empty_neighbour = true;
										}
									}
								} else {
									if(!has_empty_neighbour){
										if(cubes_are_adjacent(cube, e, dim)){
											neighbours[dim] = &e;
										}
									}
								}
							}
						}
                    }
                }
            }
        }
        return neighbours;
    }

    void define_all_neighbours(Coord size) {
        vector<Cube> old_elements;
        elements.swap(old_elements);
        for(const auto& e: old_elements){
            const vector<const Cube *> &neighbours = find_neighbours(e, size);
            elements.push_back(Cube(e, neighbours));
        }
    }

	void tree_process_box_2D(int dimension, const Cube& box) {
		cut_off_boxes.push_back(box);
	}

	const vector<Cube> get_cut_off_boxes() const {
		return cut_off_boxes;
	}

	int count_elements_within_box(const Cube &cube) {
		int count = 0;
		for (const auto& e: elements) {
			if (e.non_empty() && e.contained_in_box(cube)) count++;
		}
		return count;
	}

    Node * add_el_tree_element(Cube cube, Node * parent) {
		Node* node = new Node(cube, el_tree_node_id++);
		if (parent){
			parent->add_child(node);
		}
		el_tree_nodes.push_back(node);
		return node;
    }

	void print_tabs(int cnt) {
		for (int i = 0; i < cnt; i++)
			cout << "  ";
	}

	void try_to_tree_process(int dimension, Node * node, const string& tag, bool toggle_dim) {
		if(count_elements_within_box(node->get_cube()) > 1){
			tree_process_cut_off_box(dimension, node, toggle_dim);
		}
	}

	void tree_process_cut_off_box(int dimension, Node * node, bool toggle_dim) {
		Cube cut_off_cube = node->get_cube();
		Cube first_half, second_half;
		cut_off_cube.split_halves(dimension, &first_half, &second_half);

		Node * first_half_node = this->add_el_tree_element(first_half, node);
		Node * second_half_node = this->add_el_tree_element(second_half, node);

		if (toggle_dim){
			dimension = dimension ^ 1;
		}

		try_to_tree_process(dimension, first_half_node, "first", toggle_dim);
		try_to_tree_process(dimension, second_half_node, "second", toggle_dim);
	}

	const vector<Node*> &get_el_tree_nodes() const {
		return el_tree_nodes;
	}

private:

	void add_vertex_2D(Coord x, Coord y) {
		add_element_2D(x, x, y, y);
	}

	void add_element_2D(Coord left, Coord right, Coord up, Coord down) {
		add_element(Cube(left, right, up, down));
	}

	void add_element(const Cube& e) {
		elements.push_back(e);
	}

	Cube original_box;
	vector<Cube> elements;
	vector<Cube> cut_off_boxes;
	vector<Node*> el_tree_nodes;
    int el_tree_node_id;
};

Cube get_outmost_box(Coord size) {
	return Cube(0, size, 0, size);
}

Cube get_inner_box(Coord middle, Coord edge_offset) {
	return Cube(middle - edge_offset, middle + edge_offset,
			middle - edge_offset, middle + edge_offset);
}

int main(int argc, char** argv) {

	bool new_net = true;

	enum OutputFormat {
		GALOIS,
		GNUPLOT
	} output_format = GALOIS;

	if (argc >= 2 && string(argv[1]) == "--gnuplot") {
		output_format = GNUPLOT;
		argc--;
		argv++;
	}

	int depth = argc == 2 ? atoi(argv[1]) : 4;
	//int order = 2; // atoi(argv[2])

	Coord size = 4 << depth; // so that the smallest elements are of size 1x1
	Cube outmost_box(get_outmost_box(size));
	Domain domain(outmost_box, depth);

	// Build a regular 4x4 grid.
	domain.split_all_elements_2D();  // 1 -> 4 elements
	domain.split_all_elements_2D();  // 4 -> 16 elements
	if (depth > 1)
		domain.split_eight_side_elements_within_box_2D(outmost_box);

	Coord middle = size / 2;
	Coord edge_offset = size / 4;
	Cube outer_box = outmost_box;


	// Generate the adapted grid.
	for (int i = 1; i < depth; i++) {
		Cube inner_box(get_inner_box(middle, edge_offset));

		domain.add_edge_2D(X_DIM, outer_box, inner_box.up(),    4, new_net);  // horizontal
		domain.add_edge_2D(X_DIM, outer_box, inner_box.down(),  4, new_net);
		domain.add_edge_2D(Y_DIM, outer_box, inner_box.left(),  4, new_net);  // vertical
		domain.add_edge_2D(Y_DIM, outer_box, inner_box.right(), 4, new_net);
		domain.add_corner_vertices_2D(inner_box);

		// Internal 4 elements -> 16 elements
		domain.split_elements_within_box_2D(inner_box);
		if (i < depth-1)
			domain.split_eight_side_elements_within_box_2D(inner_box);

		edge_offset /= 2;
		outer_box = inner_box;
	}

    domain.enumerate_all_elements();
	domain.define_all_neighbours(size);

	if (output_format == GALOIS) {
		domain.print_all_elements(false /* require_non_empty */, true /* with_id */);
	} else { // output_format == GNUPLOT		
		domain.print_all_elements(false /* require_non_empty */, false /* with_id */);
	}

	if (output_format == GALOIS) {
		edge_offset = size / 4;
		outer_box = outmost_box;

        Node * outer_node = domain.add_el_tree_element(outer_box, NULL);
		Node * side_node;

		// Generate elimination tree.
		for (int i = 1; i < depth; i++) {
			Cube inner_box(get_inner_box(middle, edge_offset));
			Cube side_box, main_box;

            outer_box.split(X_DIM, inner_box.left(), &side_box, &main_box);
			side_node = domain.add_el_tree_element(side_box, outer_node);
			domain.tree_process_cut_off_box(Y_DIM, side_node, false);
			outer_node = domain.add_el_tree_element(main_box, outer_node);
			outer_box = main_box;
			domain.tree_process_box_2D(Y_DIM, side_box);
            //process side_box

            outer_box.split(X_DIM, inner_box.right(), &main_box, &side_box);
			side_node = domain.add_el_tree_element(side_box, outer_node);
			outer_node = domain.add_el_tree_element(main_box, outer_node);
			domain.tree_process_cut_off_box(Y_DIM, side_node, false);
            outer_box = main_box;
            domain.tree_process_box_2D(Y_DIM, side_box);

            outer_box.split(Y_DIM, inner_box.up(), &side_box, &main_box);
			side_node = domain.add_el_tree_element(side_box, outer_node);
			outer_node = domain.add_el_tree_element(main_box, outer_node);
			domain.tree_process_cut_off_box(X_DIM, side_node, false);
			outer_box = main_box;
            domain.tree_process_box_2D(X_DIM, side_box);

            outer_box.split(Y_DIM, inner_box.down(), &main_box, &side_box);
			side_node = domain.add_el_tree_element(side_box, outer_node);
			outer_node = domain.add_el_tree_element(main_box, outer_node);
			domain.tree_process_cut_off_box(X_DIM, side_node, false);
			outer_box = main_box;
            domain.tree_process_box_2D(X_DIM, side_box);

			edge_offset /= 2;
			if (i == depth - 1){
				domain.tree_process_cut_off_box(X_DIM, outer_node, true);
			}
		}


		cout << domain.get_cut_off_boxes().size() << endl;
		for (const Cube& box: domain.get_cut_off_boxes()) {
			box.print_full();
		}

		cout << "desired elimination tree output:" << endl;
		cout << endl << domain.get_el_tree_nodes().size() << endl;
		for (const Node* node: domain.get_el_tree_nodes()) {
			cout << node->get_num() << " ";
			cout << domain.count_elements_within_box(node->get_cube()) << " ";
			domain.print_el_lvl_id_within_box(node->get_cube());
			for (const Node* n: node->get_children()) {
				cout << n->get_num() << " ";
			}
			cout << endl;
		}
	}

	return 0;
}
