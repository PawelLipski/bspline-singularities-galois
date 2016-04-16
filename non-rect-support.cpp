
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"


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


int SIZE = 6; // in each dimension
int SAMPLE_CNT = 15; // in each dimension

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (argc == 2 && string(argv[1]) == "-s") {
		output = SCREEN;
	}

	print_grid_rect(0, 0, 2, 2, true);
	print_grid_line(0, 1, 2, 1, false);
	print_grid_line(1, 0, 1, 2, false);

	print_grid_rect(2, 2, 4, 4, true);
	print_grid_line(0, 3, 2, 3, false);
	print_grid_line(1, 2, 1, 4, false);

	print_grid_rect(0, 2, 2, 4, true);
	print_grid_line(2, 3, 4, 3, false);
	print_grid_line(3, 2, 3, 4, false);

	print_config(SIZE, SAMPLE_CNT);
	print_rotate_view(30, 30);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	string l_bspline_file = "left_bspline.dat";
	Bspline2D l_bspline({0, 0, 0, 2}, {0, 2, 2, 2}, 0.5);
	samples_2d(&l_bspline, l_bspline_file, SAMPLE_CNT);
	print_plot_command(l_bspline_file, "red", false);

	string r_bspline_file = "right_bspline.dat";
	Bspline2D r_bspline({2, 2, 2, 4}, {2, 4, 4, 4}, 0.5);
	//Bspline2D r_bspline({2, 2, 4, 4}, {2, 2, 4, 4}, 1.0);
	samples_2d(&r_bspline, r_bspline_file, SAMPLE_CNT);
	print_plot_command(r_bspline_file, "navy", true);

	string b_bspline_file = "back_bspline.dat";
	Bspline2D b_bspline({0, 0, 0, 2}, {2, 4, 4, 4}, 1.0);
	samples_2d(&b_bspline, b_bspline_file, SAMPLE_CNT);
	print_plot_command(b_bspline_file, "black", true);

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

