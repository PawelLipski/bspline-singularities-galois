#include <iostream>
#include <vector>
using namespace std;

typedef int Coord;

enum {
	X_DIM = 0,
	Y_DIM = 1
};

// A general hyper-cube for any number of dimensions.
class Cube {

	public:

	Cube() {}

	Cube(int dims): dimensions(dims) {
		limits.resize(dims * 2);
	}

	Cube(Coord l, Coord r, Coord u, Coord d): dimensions(2) {
		limits = { l, r, u, d };
	}

	bool contained_in_box(const Cube& box) const {
		for (int i = 0; i < dimensions; i++)
			if (!(box.get_from(i) <= get_from(i) && get_to(i) <= box.get_to(i)))
				return false;
		return true;
	}

	inline Coord get_from(int dimension) const {
		return limits[2*dimension];
	}

	Coord get_size(int dimension) const {
		return get_to(dimension) - get_from(dimension);
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

	void print() const {
		for (int i = 0; i < dimensions * 2; i++)
			cout << limits[i] << " ";
		cout << endl;
	}

	void set_limits(int dimension, Coord from, Coord to) {
		limits[dimension * 2] = from;
		limits[dimension * 2 + 1] = to;
	}

	Coord left()  const { return limits[0]; }
	Coord right() const { return limits[1]; }
	Coord up()    const { return limits[2]; }
	Coord down()  const { return limits[3]; }

	private:

	vector<Coord> limits;
	int dimensions;
};

class Domain {
	public:

	Domain(Coord l, Coord r, Coord u, Coord d) {
		add_element_2D(l, r, u, d);
		original_box = elements[0]; // preserve for the object lifetime
	}

	// Splits each element into 2**dimension smaller ones.
	void split_elements_within_box_2D(const Cube& box) {
		vector<Cube> old_elements;
		elements.swap(old_elements);
		for (const auto& e: old_elements) {
			if (e.non_empty() && e.contained_in_box(box)) {
				Coord x_mid = (e.left() + e.right()) / 2;
				Coord y_mid = (e.up() + e.down()) / 2;
				add_element_2D(e.left(), x_mid,  e.up(), y_mid);
				add_element_2D(x_mid, e.right(), e.up(), y_mid);
				add_element_2D(e.left(), x_mid,  y_mid, e.down());
				add_element_2D(x_mid, e.right(), y_mid, e.down());
			} else {
				add_element(e);
			}
		}
	}

	// Splits each element into 2**dimension smaller ones.
	void split_all_elements_2D() {
		split_elements_within_box_2D(original_box);
	}

	// Inserts `count' of edge elements parallel to the given dimension's axis,
	// spanning from one side of the `box' to the other in the given dimension.
	// The other dimension is fixed at `coord'.
	void add_edge_2D(int dimension, const Cube& box, Coord coord, int count) {
		int from = box.get_from(dimension);
		int to = box.get_to(dimension);
		Coord element_size = (to - from) / count;
		for (int i = 0; i < count; i++) {
			Coord element_from = from + element_size * i;
			Coord element_to = element_from + element_size;
			Cube e(2);
			e.set_limits(dimension, element_from, element_to);
			e.set_limits(dimension ^ 1, coord, coord); // the other dimension
			elements.push_back(e);
		}
	}

	// Inserts a single infinitely small element.
	void add_corner_vertices_2D(const Cube& box) {
		add_vertex_2D(box.left(), box.up());
		add_vertex_2D(box.left(), box.down());
		add_vertex_2D(box.right(), box.up());
		add_vertex_2D(box.right(), box.down());
	}

	void print_elements_within_box(const Cube& box) const {
		cout << elements.size() << endl;
		for (const auto& e: elements)
			if (e.contained_in_box(box))
				e.print();
	}

	void print_all_elements() const {
		print_elements_within_box(original_box);
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
};

int main() {
	int depth = 4; //atoi(argv[1])
	//int order = 2; // atoi(argv[2])

	int size = 2 << depth; // so that the smallest elements are of size 1x1
	Domain domain(0, size, 0, size);

	domain.split_all_elements_2D(); // 1 -> 4 elements
	domain.split_all_elements_2D(); // 4 -> 16 elements

	Coord middle = size / 2;
	Coord edge_offset = size / 4;
	Cube prev_box(0, size, 0, size);

	for (int i = 1; i < depth; i++) {
		Cube box(
			middle - edge_offset, middle + edge_offset,
			middle - edge_offset, middle + edge_offset);

		domain.add_edge_2D(X_DIM, prev_box, box.up(),    4); // horizontal
		domain.add_edge_2D(X_DIM, prev_box, box.down(),  4);
		domain.add_edge_2D(Y_DIM, prev_box, box.left(),  4); // vertical
		domain.add_edge_2D(Y_DIM, prev_box, box.right(), 4);

		domain.add_corner_vertices_2D(box);

		// Internal 4 elements -> 16 elements
		domain.split_elements_within_box_2D(box);

		edge_offset /= 2;
		prev_box = box;
	}

	domain.print_all_elements();

	return 0;
}

