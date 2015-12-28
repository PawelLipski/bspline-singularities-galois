
// A general hyper-cube for any number of dim_cnt.
class Cube {

public:

	Cube() {}

	Cube(int dims): dim_cnt(dims) {
		bounds.resize(dims * 2);
        neighbors.resize(dims * 2);
	}

	Cube(Coord l, Coord r, Coord u, Coord d): dim_cnt(2) {
		bounds = { l, r, u, d };
		neighbors.resize(4);
	}

    Cube(const Cube& cube, int n, int l):
        dim_cnt(cube.dim_cnt), bounds(cube.bounds), neighbors(cube.neighbors), level(l), num(n) {
	}


	/*** GETTERS ***/

    Coord get_bound(int bound_no) const {
        return bounds[bound_no];
    }

	inline Coord get_from(int dim) const {
		return bounds[2*dim];
	}

	Coord get_middle(int dim) const {
		return (get_from(dim) + get_to(dim)) / 2;
	}

	inline Coord get_to(int dim) const {
		return bounds[2*dim+1];
	}

	Coord get_size(int dim) const {
		return get_to(dim) - get_from(dim);
	}

	Coord left()  const { return bounds[0]; }
	Coord right() const { return bounds[1]; }
	Coord up()    const { return bounds[2]; }
	Coord down()  const { return bounds[3]; }

    int get_dim_cnt() const {
        return dim_cnt;
    }

    int get_num() const {
        return num;
    }

    int get_level() const {
        return level;
    }

	Cube* get_neighbor(int bound_no) const {
		return neighbors[bound_no];
	}

	int get_neighbor_count() const {
		int cnt = 0;
		for (int bound_no = 0; bound_no < 2 * dim_cnt; bound_no++)
			if (neighbors[bound_no] != nullptr)
				cnt++;
		return cnt;
	}

	vector<int> get_bsplines() const {
		return bsplines;
	}


	/*** SETTERS ***/

	void set_bounds(int dim, Coord from, Coord to) {
		bounds[dim * 2] = from;
		bounds[dim * 2 + 1] = to;
	}

	void set_neighbor(int bound_no, Cube* cube) {
		neighbors[bound_no] = cube;
	}


	/*** CHECKS ***/

    // Whether this cube is fully contained within the given box.
	bool contained_in_box(const Cube& box) const {
		for (int i = 0; i < dim_cnt; i++)
			if (!(box.get_from(i) <= get_from(i) && get_to(i) <= box.get_to(i)))
				return false;
		return true;
	}

	bool empty() const {
		return !non_empty();
	}

	bool is_point_2D() const {
		return get_size(X_DIM) + get_size(Y_DIM) == 0;
	}

	bool non_empty() const {
		for (int i = 0; i < dim_cnt; i++)
			if (get_size(i) == 0)
				return false;
		return true;
	}


	/*** OVERLAPPING ***/

	Coord get_overlapping_part(const Cube& other, int dim_no) const {
		Coord from = max(get_from(dim_no), other.get_from(dim_no));
		Coord to = min(get_to(dim_no), other.get_to(dim_no));
		return max(to - from, 0);
	}

	bool overlaps_with(const Cube& other) const {
		for (int dim_no = 0; dim_no < dim_cnt; dim_no++) {
			if (get_to(dim_no) <= other.get_from(dim_no))
				return false;
			if (other.get_to(dim_no) <= get_from(dim_no))
				return false;
		}
		return true;
	}


	/*** PRINTING ***/

	void print_full() const {
		print_bounds();
		print_id();
		cout << endl;
	}

	void print_id() const {
		cout << get_num()+1 << " " << get_level() << " ";
	}

	void print_bounds() const {
		for (int i = 0; i < dim_cnt * 2; i++)
			cout << bounds[i] << " ";
	}

    void print_level_id_and_bsplines() const {
        cout << get_level() << " ";
        cout << get_num()+1 << " ";
        cout << bsplines.size();
        for (int bspline: bsplines)
            cout << " " << bspline+1;
        cout << endl;
    }


	/*** SPLITTING ***/

	// Splits this cube into two cubes, along the given dimension at the given
	// coordinate, and puts the new cubes into the given pointers.
	void split(int dim, Coord coord, Cube* first, Cube* second) const {
		*first = *this;
		*second = *this;
		first->set_bounds(dim, get_from(dim), coord);
		second->set_bounds(dim, coord, get_to(dim));
	}

	void split_halves(int dim, Cube* first, Cube* second) const {
		split(dim, get_middle(dim), first, second);
	}


	/*** TWEAKING ***/

	void scale_up(int factor) {
		for (int bound_no = 0; bound_no < 2 * dim_cnt; bound_no++)
			bounds[bound_no] *= factor;		
	}

	void spread(int bound_no, int shift) {
		if (bound_no % 2 == 0)
			bounds[bound_no] -= shift; // from
		else
			bounds[bound_no] += shift; // to
	}

	void spread(int shift) {
		for (int bound_no = 0; bound_no < 2 * dim_cnt; bound_no++)
			spread(bound_no, shift);
	}

	void pump_or_squeeze(int shift) {
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

	void back_up_bounds() {
		backed_up_bounds = bounds;
	}

	void restore_bounds() {
		bounds = backed_up_bounds;
	}


	/*** B-SPLINES ***/

    vector<int> compute_bspline_support_2D() {
        vector<int> support_bounds;
        support_bounds.resize(dim_cnt * 2);
        for(int i = 0; i < dim_cnt * 2; ++i){
            if (neighbors[i]){
                support_bounds[i] = neighbors[i]->get_bound(i);
            } else {
                support_bounds[i] = bounds[i];
            }
        }
        return support_bounds;
    };

    void add_bspline(int bspline_num){
        bsplines.push_back(bspline_num);
    }

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

