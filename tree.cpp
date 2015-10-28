#include <iostream>
#include <vector>
using namespace std;

typedef int Coord;

enum {
	X_DIM = 0,
	Y_DIM = 1
};

class Element {

	public:

	Element() {}

	Element(int dims): dimensions(dims) {
		limits.resize(dims * 2);
	}

	Element(Coord l, Coord r, Coord u, Coord d): dimensions(2) {
		limits = { l, r, u, d };
	}

	bool contained_in_box_2D(Coord l, Coord r, Coord u, Coord d) const {
		return l <= left() && right() <= r && u <= up() && down() <= d;			
	}

	Coord get_size(int dimension) const {
		return limits[2*dimension+1] - limits[2*dimension];
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
		total_span = elements[0]; // preserve for the object lifetime
	}

	// Splits each element into 2**dimension smaller ones.
	void partial_cube_split_2D(Coord left, Coord right, Coord up, Coord down) {
		vector<Element> old_elements;
		elements.swap(old_elements);
		for (const auto& e: old_elements) {
			if (e.non_empty() && e.contained_in_box_2D(left, right, up, down)) {
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
	void full_cube_split_2D() {
		partial_cube_split_2D(
			total_span.left(), total_span.right(),
			total_span.up(), total_span.down());
	}

	// Inserts `count' of edge elements parallel to the given dimension's axis,
	// the other dimension fixed at `coord'.
	void insert_edge(int dimension, Coord from, Coord to, Coord coord, int count) {
		//cout << "from: " << from << ", to: " << to << ", coord: " << coord << endl;
		Coord element_size = (to - from) / count;
		for (int i = 0; i < count; i++) {
			Coord element_from = from + element_size * i;
			Coord element_to = element_from + element_size;
			Element e(2);
			e.set_limits(dimension, element_from, element_to);
			e.set_limits(dimension^1, coord, coord); // the other dimension
			elements.push_back(e);
		}
	}

	// Inserts a single infinitely small element (TODO: 2D specific so far).
	void insert_vertex_2D(Coord x, Coord y) {
		add_element_2D(x, x, y, y);
	}

	void print() const {
		cout << elements.size() << endl;
		for (const auto& e: elements)
			e.print();
	}


	private:

	void add_element_2D(Coord left, Coord right, Coord up, Coord down) {
		add_element(Element(left, right, up, down));
	}

	void add_element(const Element& e) {
		elements.push_back(e);
	}

	Element total_span;
	vector<Element> elements;
};

int main() {
	int depth = 4; //atoi(argv[1])
	//int order = 2; // atoi(argv[2])

	int size = 2 << depth; // so that the smallest elements are of size 1x1
	Domain domain(0, size, 0, size);

	domain.full_cube_split_2D(); // 1 -> 4 elements
	domain.full_cube_split_2D(); // 4 -> 16 elements

	Coord middle = size / 2;
	Coord edge_offset = size / 4;
	Coord prev_axis_0 = 0;
	Coord prev_axis_1 = size;

	for (int i = 1; i < depth; i++) {
		Coord axis_0 = middle - edge_offset;
		Coord axis_1 = middle + edge_offset;

		domain.insert_edge(X_DIM, prev_axis_0, prev_axis_1, axis_0, 4); // horizontal
		domain.insert_edge(X_DIM, prev_axis_0, prev_axis_1, axis_1, 4);
		domain.insert_edge(Y_DIM, prev_axis_0, prev_axis_1, axis_0, 4); // vertical
		domain.insert_edge(Y_DIM, prev_axis_0, prev_axis_1, axis_1, 4);

		domain.insert_vertex_2D(axis_0, axis_0);
		domain.insert_vertex_2D(axis_0, axis_1);
		domain.insert_vertex_2D(axis_1, axis_0);
		domain.insert_vertex_2D(axis_1, axis_1);

		// Internal 4 elements -> 16 elements
		domain.partial_cube_split_2D(axis_0, axis_1, axis_0, axis_1);

		edge_offset /= 2;
		prev_axis_0 = axis_0;
		prev_axis_1 = axis_1;
	}

	domain.print();

	return 0;
}

