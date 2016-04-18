
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linearCombination.h"
#include "bsplineNonRect.h"

class LinearGnomon: public Function2D {
public:
	LinearGnomon(): Function2D(Cube(0, 2, 0, 2)) {
	}

	double apply(double x, double y) const {
		if (x < 0 || 2 < x || y < 0 || 2 < y)
			return 0.0;

		x--, y--;
		if (0 < y) { // Upper half
			if (x < 0) // Left-upper quarter
				return y / 2;
			else // Right-upper quarter
				return (x + y) / 2;
		} else { // Lower half
			if (x < 0) // Left-lower quarter
				return 0;
			else // Right-lower quarter
				return x / 2;
		}
	}
};

class QuadraticGnomon: public Function2D {
public:
	QuadraticGnomon(): Function2D(Cube(0, 2, 0, 2)) {
	}

	double apply(double x, double y) const {
		if (x < 0 || 2 < x || y < 0 || 2 < y)
			return 0.0;

		x--, y--;
		if (0 < y) { // Upper half
			if (x < 0) // Left-upper quarter
				return y * y / 4;
			else // Right-upper quarter
				return (x + y) * (x + y) / 4;
		} else { // Lower half
			if (x < 0) // Left-lower quarter
				return 0;
			else // Right-lower quarter
				return x * x / 4;
		}
	}
};

class GnomonedBspline: public Function2D {
public:
	GnomonedBspline() : Function2D(Cube(4, 6, 4, 6)), bspline({0, 1, 2, 2}, {0, 1, 2, 2}, 1.0) {
	}

	double apply(double x, double y) const {
		return 12 * gnomon.apply(x - 4, y - 4) * bspline.apply(x - 4, y - 4);
	}

private:
	QuadraticGnomon gnomon;
	Bspline2D bspline;
};

int SIZE = 8; // in each dimension
int SAMPLE_CNT = 30; // in each dimension
double NOT_SCALED = 1.0;

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (argc == 2 && string(argv[1]) == "-s") {
		output = SCREEN;
	}

	print_grid_rect(0, 0, 4, 4, true);
	print_grid_line(0, 2, 4, 2, false);
	print_grid_line(2, 0, 2, 4, false);

	print_grid_rect(4, 4, 8, 8, true);
	print_grid_line(0, 6, 4, 6, false);
	print_grid_line(2, 4, 2, 8, false);

	print_grid_rect(0, 4, 4, 8, true);
	print_grid_line(4, 6, 8, 6, false);
	print_grid_line(6, 4, 6, 8, false);

	print_grid_rect(4, 4, 6, 2, true);
	print_grid_line(4, 3, 6, 3, false);
	print_grid_line(5, 2, 5, 4, false);

	print_grid_rect(6, 4, 8, 2, true);
	print_grid_line(6, 3, 8, 3, false);
	print_grid_line(7, 2, 7, 4, false);

	print_grid_rect(4, 2, 6, 0, true);
	print_grid_line(4, 1, 6, 1, false);
	print_grid_line(5, 0, 5, 2, false);

	print_config(SIZE, SAMPLE_CNT);
	print_rotate_view(30, 30);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	double CONSTANT = 0.2;

	string main_bspline_file = "main_bspline.dat";
	Bspline2DNonRect main_bspline({0, 4, 4, 8}, {0, 4, 4, 8}, {4, 4, 8, 0}, 0.8);
	double max_main = samples_2d(&main_bspline, main_bspline_file, SAMPLE_CNT);
	print_plot_command(main_bspline_file, "green", false);

	string l_bspline_file = "left_bspline.dat";
	Bspline2D l_bspline({4, 4, 4, 6}, {0, 2, 2, 2}, CONSTANT);
	double max_red = samples_2d(&l_bspline, l_bspline_file, SAMPLE_CNT);
	print_plot_command(l_bspline_file, "red", true);

	string r_bspline_file = "right_bspline.dat";
	Bspline2D r_bspline({6, 6, 6, 8}, {2, 4, 4, 4}, CONSTANT);
	double max_navy = samples_2d(&r_bspline, r_bspline_file, SAMPLE_CNT);
	print_plot_command(r_bspline_file, "navy", true);

	string b_bspline_file = "main_b_bspline.dat";

	Bspline2D b_bspline({4, 4, 4, 6}, {2, 4, 4, 4}, NOT_SCALED);
	Bspline2D b_l_bspline({4, 4, 4, 6}, {2, 2, 2, 4}, CONSTANT);
	Bspline2D b_r_bspline({4, 6, 6, 6}, {2, 4, 4, 4}, CONSTANT);
	Bspline2DLinearCombination b_bspline_combination(b_bspline, b_l_bspline, b_r_bspline, NOT_SCALED);


	double max_black = samples_2d(&b_bspline_combination, b_bspline_file, SAMPLE_CNT);
	print_plot_command(b_bspline_file, "black", true);

	cout << endl;
	if (output == SCREEN)
		print_pause();

	//cout << "max red: " << max_red << ", max navy: " << max_navy << ", max black: " << max_black << endl;
}

