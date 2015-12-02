
class Domain {

public:

	Domain(const Cube& box) {
		add_element(box);
		original_box = box;  // preserve for the object lifetime
	}


	/*** CHECKS ***/

	bool is_middle_element(const Cube &cube, Coord middle) const {
		return (cube.right() == middle || cube.left() == middle) &&
			   (cube.up() == middle || cube.down() == middle);
	}

	bool is_horizontal_side_element(const Cube &cube, Coord middle) const {
		return (cube.down() == middle) || (cube.up() == middle);
	}

	bool is_vertical_side_element(const Cube &cube, Coord middle) const {
		return (cube.right() == middle) || (cube.left() == middle);
	}

	bool overlaps_with_any_other(const Cube& that) const {
		for (const auto& other: elements)
			if (&that != &other && that.overlaps_with(other))
				return true;
		return false;
	}


	/*** SPLIT ELEMENTS ***/

	void split_eight_side_elements_within_box_2D(Cube cube) {
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


	/*** ADD ELEMENTS ***/

	// Inserts `count' of edge elements parallel to the given dimension's axis,
	// spanning from one side of the `box' to the other in the given dimension.
	// The other dimension is fixed at `coord'.
	void add_edge_2D(int dim, const Cube& box, Coord coord, int count, bool edged_8) {
		int from = box.get_from(dim);
		int to = box.get_to(dim);
		Coord element_size = (to - from) / count;
		for (int i = 0; i < count; i++) {
			Coord element_from = from + element_size * i;
			Coord element_to = element_from + element_size;

			//needs a flag, only for the new mesh
			if (edged_8) {
				if (i == count / 2 || i == (count / 2 - 1)){
					Cube e1(2), e2(2);
					Coord mid = (element_from + element_to) / 2;
					e1.set_bounds(dim, element_from, mid);
					e1.set_bounds(dim ^ 1, coord, coord);  // the other dimension
					elements.push_back(e1);
					e2.set_bounds(dim, mid, element_to);
					e2.set_bounds(dim ^ 1, coord, coord);  // the other dimension
					elements.push_back(e2);
					if(dim == X_DIM){
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
	void add_corner_vertices_2D(const Cube& box) {
		add_vertex_2D(box.left(), box.up());
		add_vertex_2D(box.left(), box.down());
		add_vertex_2D(box.right(), box.up());
		add_vertex_2D(box.right(), box.down());
	}


	/*** PRINT ELEMENTS ***/

	void print_elements_within_box(const Cube& box, bool require_non_empty, bool with_id) const {
		cout << elements.size() << endl;
		for (const auto& e: elements) {
			if (require_non_empty && e.empty())
				continue;
			if (!e.contained_in_box(box))
				continue;

			e.print_bounds();
			if (with_id)
				e.print_id();
			cout << endl;
		}
	}

	void print_elements_level_and_id_within_box(const Node *node) const {
		for (const auto& e: elements) {
			if (e.non_empty() && e.contained_in_box(node->get_cube())) {
				cout << e.get_level() << " " << e.get_num() << " ";
			}
		}
	}

	void print_all_elements(bool require_non_empty, bool with_id) const {
		print_elements_within_box(original_box, require_non_empty, with_id);
	}

    void println_non_empty_elements_count() const {
        cout << count_non_empty_elements() << endl;
    }

    void print_el_tree_nodes_count() const {
        cout << get_el_tree_nodes().size() << endl;
    }

    void print_galois_output() const {
        print_b_splines_line_by_line();
        print_b_splines_per_elements();
        print_elements_per_el_tree_nodes();
    }

	static void print_line(Coord x1, Coord y1, Coord x2, Coord y2) {
		cout << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
	}

	static void print_tabs(int cnt) {
		for (int i = 0; i < cnt; i++)
			cout << "  ";
	}


	/*** NEIGHBORS ***/

    bool cubes_are_adjacent(const Cube& that, const Cube& other, int bound_no, bool looseened_conds) const {
		int given_dim_no = bound_no >> 1;
        int opposite_bound_no = bound_no ^ 1;
		if (that.get_bound(bound_no) != other.get_bound(opposite_bound_no))
			return false;

		for (int dim_no = 0; dim_no < that.get_dim_cnt(); dim_no++) {
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

	void print_all_neighbors() const {
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

    void compute_neighbors(Cube& that, int size) {
        // Check regular neighbors.
        for (auto& other: elements)
			for (int bound_no = 0; bound_no < that.get_dim_cnt() * 2; bound_no++)
				if (cubes_are_adjacent(that, other, bound_no, false))
					that.set_neighbor(bound_no, &other);
        // Check extra neigbors if regular are not found.
        for (int bound_no = 0; bound_no < that.get_dim_cnt() * 2; bound_no++) {
            if(that.get_neighbor(bound_no) == nullptr &&
                    that.get_bound(bound_no) != 0 && that.get_bound(bound_no) != size) {
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


	/*** TREE ***/

	void tree_process_box_2D(const Cube& box) {
		cut_off_boxes.push_back(box);
	}

	vector<Cube> get_cut_off_boxes() const {
		return cut_off_boxes;
	}

	int count_elements_within_box(const Cube &cube) const {
		int count = 0;
		for (const auto& e: elements)
			if (e.non_empty() && e.contained_in_box(cube)) 
				count++;
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

	void try_to_tree_process(int dim, Node * node, bool toggle_dim) {
		if (count_elements_within_box(node->get_cube()) > 1){
			tree_process_cut_off_box(dim, node, toggle_dim);
		}
	}

	void tree_process_cut_off_box(int dim, Node * node, bool toggle_dim) {
		Cube cut_off_cube = node->get_cube();
		Cube first_half, second_half;
		cut_off_cube.split_halves(dim, &first_half, &second_half);

		Node* first_half_node = this->add_el_tree_element(first_half, node);
		Node* second_half_node = this->add_el_tree_element(second_half, node);

		if (toggle_dim)
			dim ^= 1;

		try_to_tree_process(dim, first_half_node, toggle_dim);
		try_to_tree_process(dim, second_half_node, toggle_dim);
	}

	const vector<Node*> &get_el_tree_nodes() const {
		return el_tree_nodes;
	}

    void print_elements_per_el_tree_nodes() const {
        print_el_tree_nodes_count();
        for (const Node* node: get_el_tree_nodes()) {
            node->print_num();
            print_elements_count_within_node(node);
            print_elements_level_and_id_within_box(node);
            print_node_children(node);
        }
    }

	void print_el_tree_size() const { 
		cout << get_cut_off_boxes().size() << endl; 
	}

	void print_el_tree_for_draw() const {
		print_el_tree_size();
		for (const Cube& box: get_cut_off_boxes()) {
			box.print_full();
		}
	}

    void print_node_children(const Node *node) const {
        for (const Node* n: node->get_children()) {
            cout << n->get_num() << " ";
        }
        cout << endl;
    }

    void print_elements_count_within_node(const Node *node) const {
        cout << count_elements_within_box(node->get_cube()) << " ";
    }


	/*** BOUNDARY TWEAKING ***/

	void tweak_bounds() {
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
	
	void untweak_bounds() {
		for (auto& e: elements)
			e.restore_bounds();
		original_box.restore_bounds();
	}


	/*** B-SPLINES ***/

    void compute_b_splines_supports(bool print) {
		if (print)
			cout << elements.size() << endl;
        for (auto& e: elements) {
			vector<int> support;
            const vector<int> &support_bounds = e.compute_b_spline_support_2D();
			if (print)
				cout << e.get_middle(X_DIM) << " " << e.get_middle(Y_DIM) << " ";
            const Cube &support_cube = Cube(support_bounds[0], support_bounds[1], support_bounds[2], support_bounds[3]);
            for (auto& support_candidate: elements) {
                if (support_candidate.non_empty() && support_candidate.contained_in_box(support_cube)) {
                    support_candidate.add_b_spline(e.get_num());
					support.push_back(support_candidate.get_num());
                }
            }
			if (print) {
				cout << support.size() << " ";
				for (auto& s: support)
					cout << s << " ";
				cout << endl;
			}
        }
    }

	/*
	void print_support_for_each_b_spline() const {
		vector<vector<int>> supports;
        for (const auto& e: elements) {
			for (int b_spline: e.get_b_splines()) {
				supports[b_spline].push_back(e.
		}
	}
	*/

    void print_b_splines_per_elements() const {
        println_non_empty_elements_count();
        for(auto& e : elements)
            if (e.non_empty())
                e.print_level_id_and_b_splines();
    }

    void print_b_splines_line_by_line() const {
        cout << elements.size() << endl;
        for (const auto& e: elements)
            cout << e.get_num() << " " << 1 << endl;        
    }


	/*** UTILS ***/

    int count_non_empty_elements() const {
        int count = 0;
        for (const auto& e: elements)
            if (e.non_empty())
                count++;
        return count;
    }

    int compute_level(const Cube &cube) const {
        int size = max(cube.get_size(0), cube.get_size(1));
        return (int) log2((original_box.get_size(0) / size)) - 1;
    }

    void enumerate_all_elements() {
        vector<Cube> old_elements;
        elements.swap(old_elements);
        int i = 0;
        for (const auto& e: old_elements) {
            if (e.non_empty()){
                elements.push_back(Cube(e, i++, compute_level(e)));
            } else {
                elements.push_back(Cube(e, i++, -1));
            }
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

