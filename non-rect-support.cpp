
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"


class GnomonShaped: public Function2D {
public:
	GnomonShaped(): Function2D(Cube(0, 2, 0, 2)) {
	}

	double apply(double x, double y) const {
		if (x < 0 || 2 < x || y < 0 || 2 < y)
			return 0.0;

		if (1 < y) { // Upper half
			if (x < 1) // Left-upper quarter
				return y - 1;
			else // Right-upper quarter
				return x + y - 2;
		} else { // Lower half
			if (x < 1) // Left-lower quarter
				return 0;
			else // Right-lower quarter
				return x - 1;
		}
	}
};


int SIZE = 4; // in each dimension
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
	print_grid_line(2, 3, 4, 3, false);
	print_grid_line(3, 2, 3, 4, false);

	print_config(SIZE, SAMPLE_CNT);
	print_rotate_view(30, 15);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	string gnomon_file = "gnomon.dat";
	GnomonShaped gnomon;
	samples_2d(&gnomon, gnomon_file, SAMPLE_CNT);
	print_plot_command(gnomon_file, "red", false);

	string bspline_file = "bspline.dat";
	Bspline2D bspline({2, 3, 3, 4}, {2, 3, 3, 4});
	samples_2d(&bspline, bspline_file, SAMPLE_CNT);
	print_plot_command(bspline_file, "green", true);

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

