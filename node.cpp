
class Node {

public:

    Node(): num(-1) { }

    Node(const Cube& node_cube, int node_num): 
		cube(node_cube), num(node_num) {
    }

	void add_child(Node* child) {
		children.push_back(child);
	}

	const vector<Node*>& get_children() const {
		return children;
	}

	const Cube& get_cube() const {
		return cube;
	}

	int get_num() const {
		return num;
	}

    void print_num() const{
        cout << get_num() << " ";
    }

private:

	Cube cube;
	int num;
	vector<Node*> children;
};

