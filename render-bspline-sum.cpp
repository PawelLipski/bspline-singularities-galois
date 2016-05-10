
#include <climits>
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linear-combination.h"
#include "bspline-non-rect.h"

int SAMPLE_CNT = 31; // in each dimension

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
	if (argc == 1 || string(argv[1]) == "-s") 
		output = SCREEN;

	int size = generate_and_render_grid(3);

	int N; // number of elements
	cin >> N;
	for (int i = 0; i < N; i++) {
		Bounds b;
		cin >> b.left >> b.right >> b.up >> b.down;
	}

	print_config(size, SAMPLE_CNT);
	print_rotate_view(30, 45);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	int M; // number of B-splines
	cin >> M;
	vector<Function2D*> unscaled_bsplines;
	for (int i = 0; i < M; i++) {
		string type;
		// Regular or Gnomon
		cin >> type;
		if (type == "Regular") {
			vector<double> x_knots, y_knots;
			read_vector(&x_knots, 4);
			read_vector(&y_knots, 4);
			Bspline* regular = new Bspline(x_knots, y_knots);
			unscaled_bsplines.push_back(regular);
		} else {  // type == "Gnomon"
			double x_mid, y_mid, shift_x, shift_y;
			cin >> x_mid >> y_mid >> shift_x >> shift_y;
			GnomonBspline* gnomon = new GnomonBspline(x_mid, y_mid, shift_x, shift_y);
			unscaled_bsplines.push_back(gnomon);
		}
	}

	LinearCombination sum_of_unscaled(unscaled_bsplines);
	vector<Function2D*> scaled_bsplines;
	for (const Function2D* unscaled_bspline: unscaled_bsplines) {
		Quotient* scaled_bspline = new Quotient(unscaled_bspline, &sum_of_unscaled);
		scaled_bsplines.push_back(scaled_bspline);
	}
	LinearCombination sum_of_scaled(scaled_bsplines);
	string sum_file = "bspline_sum.dat";
	Rect support(0, size, 0, size);
	samples_2d(sum_of_scaled, support, sum_file, SAMPLE_CNT);
	print_plot_command(sum_file, "red", false);

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

