
#include <climits>
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linear-combination.h"
#include "bspline-non-rect.h"

int SAMPLE_CNT = 63; // in each dimension

struct Bounds {
	int left, right, up, down;
};

void read_vector(vector<double>* out, int cnt) {
	for (int i = 0; i < cnt; i++) {
		double x;
		cin >> x;
		out->push_back(x);
	}
}

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (string(argv[1]) == "-s") {
		output = SCREEN;
	}
	int N; // number of elements
	cin >> N;
	vector<Bounds> bs;
	int size = 0;
	for (int i = 0; i < N; i++) {
		Bounds b;
		cin >> b.left >> b.right >> b.up >> b.down;
		bs.push_back(b);
		size = max(size, max(b.right, b.down));
	}
	int x_from = INT_MAX, x_to = 0, y_from = INT_MAX, y_to = 0;
	for (int i = 0; i < N; i++) {
		const Bounds& b = bs[i];
		int left = b.left, right = b.right, up = b.up, down = b.down;
		if (left == right && up == down)
			continue;  // skip vertices
		bool hl = left == right || up == down;  // highlight double edges
		print_grid_line(left,  up,   right, up,   hl);
		print_grid_line(right, up,   right, down, hl);
		print_grid_line(right, down, left,  down, hl);
		print_grid_line(left,  down, left,  up,   hl);
		x_from = min(x_from, left);
		x_to = max(x_to, right);
		y_from = min(y_from, up);
		y_to = max(y_to, down);
	}
	print_config(size, SAMPLE_CNT);
	print_rotate_view(30, 45);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	int M; // number of B-splines
	cin >> M;
	vector<Function2D*> bsplines;
	for (int i = 0; i < M; i++) {
		vector<double> x_knots, y_knots;
		read_vector(&x_knots, 4);
		read_vector(&y_knots, 4);
		Bspline* bspline = new Bspline(x_knots, y_knots);
		bsplines.push_back(bspline);
	}

	string sum_file = "bspline_sum.dat";
	LinearCombination sum_of_all(bsplines);
	Rect support(x_from, x_to, y_from, y_to);
	samples_2d(sum_of_all, support, sum_file, SAMPLE_CNT);
	print_plot_command(sum_file, "red", false);

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

