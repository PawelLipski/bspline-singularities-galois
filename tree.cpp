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
		bounds.resize(dims * 2);
        neighbors.resize(dims * 2);
	}

	Cube(Coord l, Coord r, Coord u, Coord d): dimensions(2) {
		bounds = { l, r, u, d };
		neighbors.resize(4);
	}

    Cube(const Cube& cube, int n, int l):
        dimensions(cube.dimensions), bounds(cube.bounds), num(n) , lvl(l), neighbors(cube.neighbors){
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
		return bounds[2*dimension];
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
		return bounds[2*dimension+1];
	}

	bool non_empty() const {
		for (int i = 0; i < dimensions; i++)
			if (get_size(i) == 0)
				return false;
		return true;
	}

	void print_full() const {
		print_bounds();
		print_id();
		cout << endl;
	}

	void print_id() const {
		cout << num << " " << get_lvl() << " ";
	}

	void print_bounds() const {
		for (int i = 0; i < dimensions * 2; i++)
			cout << bounds[i] << " ";
	}

	void set_bounds(int dimension, Coord from, Coord to) {
		bounds[dimension * 2] = from;
		bounds[dimension * 2 + 1] = to;
	}

	// Splits this cube into two cubes, along the given dimension at the given
	// coordinate, and puts the new cubes into the given pointers.
	void split(int dimension, Coord coord, Cube* first, Cube* second) const {
		*first = *this;
		*second = *this;
		first->set_bounds(dimension, get_from(dimension), coord);
		second->set_bounds(dimension, coord, get_to(dimension));
	}

	void split_halves(int dimension, Cube* first, Cube* second) const {
		split(dimension, get_middle(dimension), first, second);
	}

	Coord left()  const { return bounds[0]; }
	Coord right() const { return bounds[1]; }
	Coord up()    const { return bounds[2]; }
	Coord down()  const { return bounds[3]; }

    Coord get_bound(int bound_no) const {
        return bounds[bound_no];
    }

	Cube* get_neighbor(int bound_no) const {
		return neighbors[bound_no];
	}

	int get_neighbor_count() const {
		int cnt = 0;
		for (int bound_no = 0; bound_no < 2 * dimensions; bound_no++)
			if (neighbors[bound_no] != nullptr)
				cnt++;
		return cnt;
	}

	void set_neighbor(int bound_no, Cube* cube) {
		neighbors[bound_no] = cube;
	}

	void scale_up(int factor) {
		for (int bound_no = 0; bound_no < 2 * dimensions; bound_no++)
			bounds[bound_no] *= factor;		
	}

	void spread(int bound_no, int shift) {
		if (bound_no % 2 == 0)
			bounds[bound_no] -= shift; // from
		else
			bounds[bound_no] += shift; // to
	}

	void spread(int shift) {
		for (int bound_no = 0; bound_no < 2 * dimensions; bound_no++)
			spread(bound_no, shift);
	}

	void pump_or_squeeze(int shift) {
		for (int dim_no = 0; dim_no < dimensions; dim_no++) {
			if (get_size(dim_no) == 0) {
				spread(2*dim_no,   +shift);
				spread(2*dim_no+1, +shift);
			} else {
				spread(2*dim_no,   -shift);
				spread(2*dim_no+1, -shift);
			}
		}
	}

	Coord get_overlapping_part(const Cube& other, int dim_no) const {
		Coord from = max(get_from(dim_no), other.get_from(dim_no));
		Coord to = min(get_to(dim_no), other.get_to(dim_no));
		return max(to - from, 0);
	}

    vector<int> compute_b_spline_support_2D() {
        vector<int> support_bounds;
        support_bounds.resize(dimensions * 2);
        for(int i = 0; i < dimensions * 2; ++i){
            if (neighbors[i]){
                support_bounds[i] = neighbors[i]->get_bound(i);
            } else {
                support_bounds[i] = bounds[i];
            }
        }
        return support_bounds;
    };

    void add_b_spline(int b_spline_num){
        b_splines.push_back(b_spline_num);
    }

    void print_lvl_id_and_b_splines(){
        cout << get_lvl() << " ";
        cout << get_num() << " ";
        cout << b_splines.size();
        for (int b_spline: b_splines)
            cout << " " << b_spline;
        cout << endl;
    }

	private:

	// Number of dimensions.
	int dimensions;
	vector<Coord> bounds;
	vector<Coord> backed_up_bounds;
public:
    vector<Cube*> neighbors;
    vector<int> b_splines;


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

	bool overlaps_with(const Cube& other) const {
		for (int dim_no = 0; dim_no < dimensions; dim_no++) {
			if (get_to(dim_no) <= other.get_from(dim_no))
				return false;
			if (other.get_to(dim_no) <= get_from(dim_no))
				return false;
		}
		return true;
	}

	void back_up_bounds() {
		backed_up_bounds = bounds;
	}

	void restore_bounds() {
		bounds = backed_up_bounds;
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

    void print_num() const{
        cout << get_num() << " ";
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
	void add_edge_2D(int dimension, const Cube& box, Coord coord, int count, bool new_mesh) {
		int from = box.get_from(dimension);
		int to = box.get_to(dimension);
		Coord element_size = (to - from) / count;
		for (int i = 0; i < count; i++) {
			Coord element_from = from + element_size * i;
			Coord element_to = element_from + element_size;

			//needs a flag, only for the new mesh
			if(new_mesh){
				if (i == count / 2 || i == (count / 2 - 1)){
					Cube e1(2), e2(2);
					Coord mid = (element_from + element_to) / 2;
					e1.set_bounds(dimension, element_from, mid);
					e1.set_bounds(dimension ^ 1, coord, coord);  // the other dimension
					elements.push_back(e1);
					e2.set_bounds(dimension, mid, element_to);
					e2.set_bounds(dimension ^ 1, coord, coord);  // the other dimension
					elements.push_back(e2);
					if(dimension == X_DIM){
						add_vertex_2D(mid, coord);
					} else {
						add_vertex_2D(coord, mid);
					}
				} else {
					Cube e(2);
					e.set_bounds(dimension, element_from, element_to);
					e.set_bounds(dimension ^ 1, coord, coord);  // the other dimension
					elements.push_back(e);
				}
			} else {
				Cube e(2);
				e.set_bounds(dimension, element_from, element_to);
				e.set_bounds(dimension ^ 1, coord, coord);  // the other dimension
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
			//if (!e.contained_in_box(box))
			//	continue;

			e.print_bounds();
			if (with_id)
				e.print_id();
			cout << endl;
		}
	}

	void print_elements_lvl_and_id_within_box(const Node *node){
		for (const auto& e: elements) {
			if(e.non_empty() && e.contained_in_box(node->get_cube())){
				cout << e.get_lvl() << " " << e.get_num() << " ";
			}
		}
	}

	void print_all_elements(bool require_non_empty, bool with_id) const {
		print_elements_within_box(original_box, require_non_empty, with_id);
	}

	void print_line(Coord x1, Coord y1, Coord x2, Coord y2) const {
		cout << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
	}

	void print_all_neighbors() const {
		
		int total_cnt = 0;
		for (const Cube& e: elements)
			total_cnt += e.get_neighbor_count();
		cout << total_cnt << endl;
		for (const Cube& that: elements) {
            //cout << "current node: ";
            //that.print_full();
			for (int bound_no = 0; bound_no < that.get_dimensions() * 2; bound_no++) {
				Cube* other = that.get_neighbor(bound_no);
				if (other != nullptr) {
					print_line(
						that.get_middle(X_DIM), that.get_middle(Y_DIM),
						other->get_middle(X_DIM), other->get_middle(Y_DIM)
					);
                    //other->print_full();
				}
			}
		}
	}

    int compute_lvl(const Cube &cube) {
        int size = max(cube.get_size(0), cube.get_size(1));
        //return (int) log2(size) + 1;
        return (int) log2((original_box.get_size(0) / size)) - 1;
    }

    void enumerate_all_elements() {
        vector<Cube> old_elements;
        elements.swap(old_elements);
        int i = 0;
        for (const auto& e: old_elements) {
            if (e.non_empty()){
                elements.push_back(Cube(e, i++, compute_lvl(e)));
            } else {
                elements.push_back(Cube(e, i++, -1));
            }
        }
    }

    bool cubes_are_adjacent(const Cube& that, const Cube& other, int bound_no, bool looseened_conds) const {
		int given_dim_no = bound_no >> 1;
        int opposite_bound_no = bound_no ^ 1;
		if (that.get_bound(bound_no) != other.get_bound(opposite_bound_no))
			return false;

		for (int dim_no = 0; dim_no < that.get_dimensions(); dim_no++) {
			if (dim_no == given_dim_no)
				continue;

			Coord overlap = that.get_overlapping_part(other, dim_no);
			if (!looseened_conds){
                if (overlap != that.get_size(dim_no) && overlap != other.get_size(dim_no))
                    return false;
            } else {
                if (overlap == 0)
                    return false;
            }
		}
		return true;
    }


    void compute_neighbors(Cube& that, int size) {
        //neighbors.resize(cube.get_dimensions() * 2);
        //check regular neighbors
        for (auto& other: elements)
			for (int bound_no = 0; bound_no < that.get_dimensions() * 2; bound_no++)
				if (cubes_are_adjacent(that, other, bound_no, false))
					that.set_neighbor(bound_no, &other);
        //check extra neigbors if regular are not found
        for (int bound_no = 0; bound_no < that.get_dimensions() * 2; bound_no++){
            if(that.get_neighbor(bound_no) == nullptr &&
                    that.get_bound(bound_no) != 0 && that.get_bound(bound_no) != size){
                for (auto& other: elements){
                    if (cubes_are_adjacent(that, other, bound_no, true))
                        that.set_neighbor(bound_no, &other);
                }
            }
        }
    }

    void compute_all_neighbors(int size) {
        for (auto& e: elements)
			compute_neighbors(e, size);
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
		if (count_elements_within_box(node->get_cube()) > 1){
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

	bool overlaps_with_any_other(const Cube& that) const {
		for (const auto& other: elements)
			if (&that != &other && that.overlaps_with(other))
				return true;
		return false;
	}

	void tweak_coords() {
		for (auto& e: elements)
			e.back_up_bounds();

		// Scale up all elements.
		for (auto& e: elements)
			e.scale_up(8);

		// Squeeze non-empty dims, pump up empty dims.
		for (auto& e: elements)
			e.pump_or_squeeze(2);

		// Try pump back non-empty dims (only if they won't overlap with now pumped-up empty dims).
		for (auto& e: elements) {
			for (int bound_no = 0; bound_no < 2 * e.get_dimensions(); bound_no++) {
				if (e.get_size(bound_no / 2) == 2)
					continue;
				e.spread(bound_no, 2);
				if (overlaps_with_any_other(e)) {
					e.spread(bound_no, -2);
				}
			}
		}
	}
	
	void untweak_coords() {
		for (auto& e: elements)
			e.restore_bounds();
	}

    void compute_b_splines_supports(bool print) {
		if (print)
			cout << elements.size() << endl;
        for (auto& e: elements) {
			vector<int> support;
            const vector<int> &support_bounds = e.compute_b_spline_support_2D();
			if (print)
				cout << e.get_middle(X_DIM) << " " << e.get_middle(Y_DIM) << " ";
            const Cube &support_cube = Cube(support_bounds[0], support_bounds[1], support_bounds[2], support_bounds[3]);
			int sup_index = 0;
            for (auto& support_candidate: elements) {
                if (support_candidate.non_empty() && support_candidate.contained_in_box(support_cube)) {
                    support_candidate.add_b_spline(e.get_num());
					support.push_back(sup_index);
                }
				sup_index++;
            }
			if (print) {
				cout << support.size() << " ";
				for (auto& s: support)
					cout << s << " ";
				cout << endl;
			}
        }
    }

    void print_b_splines_per_elements() {
        println_non_empty_elements_count();
        for(auto& e : elements){
            if (e.non_empty()){
                e.print_lvl_id_and_b_splines();
            }
        }
    }

    void print_b_splines_line_by_line() {
        cout << elements.size() << endl;
        for (const auto &e: elements){
            cout << e.get_num() << " " << 1 << endl;
        }
    }

    int count_non_empty_elements() {
        int count = 0;
        for (const auto &e: elements)
            if (e.non_empty())
                count++;
        return count;
    }

    void println_non_empty_elements_count(){
        cout << count_non_empty_elements() << endl;
    }

    void print_el_tree_nodes_count() {
        cout << get_el_tree_nodes().size() << endl;
    }

    void print_elements_count_within_node(const Node *node){
        cout << count_elements_within_box(node->get_cube()) << " ";
    }

    void print_node_children(const Node *node){
        for (const Node* n: node->get_children()) {
            cout << n->get_num() << " ";
        }
        cout << endl;
    }

    void print_elements_per_el_tree_nodes() {
        print_el_tree_nodes_count();
        for (const Node* node: get_el_tree_nodes()) {
            node->print_num();
            print_elements_count_within_node(node);
            print_elements_lvl_and_id_within_box(node);
            print_node_children(node);
        }
    }

    void print_galois_output() {
        print_b_splines_line_by_line();
        print_b_splines_per_elements();
        print_elements_per_el_tree_nodes();
    }

	void print_el_tree_size() const { cout << get_cut_off_boxes().size() << endl; }

	void print_el_tree_for_draw() {
		print_el_tree_size();
		for (const Cube& box: get_cut_off_boxes()) {
			box.print_full();
		}
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
    int el_tree_node_id = 0;
};

Cube get_outmost_box(Coord size) {
	return Cube(0, size, 0, size);
}

Cube get_inner_box(Coord middle, Coord edge_offset) {
	return Cube(middle - edge_offset, middle + edge_offset,
			middle - edge_offset, middle + edge_offset);
}

int main(int argc, char** argv) {

	bool new_mesh = true;

	enum OutputFormat {
		DRAW_NEIGHBORS,
		DRAW_SUPPORTS,
		GALOIS,
		GNUPLOT
	} output_format = GALOIS;

	if (argc >= 2) {
		string opt(argv[1]);
		if (opt == "-n" || opt == "--draw-neighbors")
			output_format = DRAW_NEIGHBORS;
		else if (opt == "-s" || opt == "--draw-supports")
			output_format = DRAW_SUPPORTS;
		else if (opt == "-g" || opt == "--galois")
			output_format = GALOIS;
		else if (opt == "-p" || opt == "--gnuplot")
			output_format = GNUPLOT;
		argc--;
		argv++;
	}

	int depth = argc == 2 ? atoi(argv[1]) : 3;
	//int order = 2; // atoi(argv[2])

	Coord size = (output_format == GALOIS ? 4 : 2) << depth; // so that the smallest elements are of size 1x1
	Cube outmost_box(get_outmost_box(size));
	Domain domain(outmost_box, depth);

	// Build a regular 4x4 grid.
	domain.split_all_elements_2D();  // 1 -> 4 elements
	domain.split_all_elements_2D();  // 4 -> 16 elements
	if (depth > 1 && new_mesh)
		domain.split_eight_side_elements_within_box_2D(outmost_box);

	Coord middle = size / 2;
	Coord edge_offset = size / 4;
	Cube outer_box = outmost_box;


	// Generate the adapted grid.
	for (int i = 1; i < depth; i++) {
		Cube inner_box(get_inner_box(middle, edge_offset));

		domain.add_edge_2D(X_DIM, outer_box, inner_box.up(),    4, new_mesh);  // horizontal
		domain.add_edge_2D(X_DIM, outer_box, inner_box.down(),  4, new_mesh);
		domain.add_edge_2D(Y_DIM, outer_box, inner_box.left(),  4, new_mesh);  // vertical
		domain.add_edge_2D(Y_DIM, outer_box, inner_box.right(), 4, new_mesh);
		domain.add_corner_vertices_2D(inner_box);

		// Internal 4 elements -> 16 elements
		domain.split_elements_within_box_2D(inner_box);
		if (i < depth-1 && new_mesh)
			domain.split_eight_side_elements_within_box_2D(inner_box);

		edge_offset /= 2;
		outer_box = inner_box;
	}

    domain.enumerate_all_elements();
    domain.tweak_coords();
	domain.compute_all_neighbors(size);

	if (output_format == DRAW_NEIGHBORS) {
        domain.print_all_elements(false, false);
        domain.print_all_neighbors();

	} else if (output_format == DRAW_SUPPORTS) {
    	domain.untweak_coords();
        domain.print_all_elements(false, false);
    	domain.compute_b_splines_supports(true);

	} else if (output_format == GALOIS) {
    	domain.untweak_coords();
    	domain.compute_b_splines_supports(false);

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

        domain.print_galois_output();

	} else if (output_format == GNUPLOT) {
    	domain.untweak_coords();
        domain.print_all_elements(false, false);
	}


	return 0;
}
