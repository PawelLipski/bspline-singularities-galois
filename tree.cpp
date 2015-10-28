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

	Coord get_size(int dimension) const {
		return limits[2*dimension+1] - limits[2*dimension];
	}

	void print() const {
		for (int i = 0; i < dimensions * 2; i++)
			cout << limits[i] << " ";
		cout << endl;
	}

	void set_limits(int dimension, int from, int to) {
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
		add_element(l, r, u, d);
		total_span = elements[0]; // preserve for the object lifetime
	}

	// Splits each element into 2**dimension smaller ones.
	void full_cube_split() {
		vector<Element> old_elements;
		elements.swap(old_elements);
		for (const auto& e: old_elements) {
			Coord x_mid = (e.left() + e.right()) / 2;
			Coord y_mid = (e.up() + e.down()) / 2;
			add_element(e.left(), x_mid,  e.up(), y_mid);
			add_element(x_mid, e.right(), e.up(), y_mid);
			add_element(e.left(), x_mid,  y_mid, e.down());
			add_element(x_mid, e.right(), y_mid, e.down());
		}
	}

	// Inserts `count' of edge elements parallel to the given dimension's axis,
	// the other dimension fixed at `coord'.
	void insert_edge(int dimension, Coord coord, int count) {
		Coord element_size = total_span.get_size(dimension) / count;
		for (int i = 0; i < count; i++) {
			Coord from = element_size * i;
			Coord to = element_size * (i + 1);
			Element e(2);
			e.set_limits(dimension, from, to);
			e.set_limits(dimension^1, coord, coord); // the other dimension
			elements.push_back(e);
		}
	}

	// Inserts a single infinitely small element (TODO: 2D specific so far).
	void insert_point(Coord x, Coord y) {
		add_element(x, x, y, y);
	}

	void print() const {
		cout << elements.size() << endl;
		for (const auto& e: elements)
			e.print();
	}


	private:

	void add_element(Coord left, Coord right, Coord up, Coord down) {
		elements.push_back(Element(left, right, up, down));
	}

	Element total_span;
	vector<Element> elements;
};

int main() {
	int depth = 2; // atoi(argv[1])
	//int order = 2; // atoi(argv[2])

	int size = depth << 2; // so that the smallest elements are of size 1x1
	Domain domain(0, size, 0, size);

	domain.full_cube_split(); // 1 -> 4 elements
	domain.full_cube_split(); // 4 -> 16 elements

	int middle = size / 2;
	int edge_offset = size / 4;
	domain.insert_edge(X_DIM, middle - edge_offset, 4); // horizontal
	domain.insert_edge(X_DIM, middle + edge_offset, 4);
	domain.insert_edge(Y_DIM, middle - edge_offset, 4); // vertical
	domain.insert_edge(Y_DIM, middle + edge_offset, 4);
	domain.insert_point(middle - edge_offset, middle - edge_offset);
	domain.insert_point(middle - edge_offset, middle + edge_offset);
	domain.insert_point(middle + edge_offset, middle - edge_offset);
	domain.insert_point(middle + edge_offset, middle + edge_offset);

	domain.print();

	return 0;
}

