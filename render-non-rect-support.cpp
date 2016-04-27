
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linear-combination.h"
#include "bspline-non-rect.h"

int SIZE = 8; // in each dimension
int SAMPLE_CNT = 40; // in each dimension

void plot(const Function2D& f, const Rect& area, string color) {
	static bool replot = false;

	string file = "plot-" + color + ".dat";
	samples_2d(f, area, file, SAMPLE_CNT);
	print_plot_command(file, color, replot);
	replot = true;
}

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (argc == 2 && string(argv[1]) == "-s") {
		output = SCREEN;
	}

	print_grid_rect(0, 0, 4, 4, true);
	print_grid_rect(4, 0, 8, 4, true);
	print_grid_rect(4, 4, 8, 8, true);
	print_grid_rect(0, 4, 4, 8, true);

	print_config(SIZE, SAMPLE_CNT);
	print_rotate_view(30, 330);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	GnomonBsplineCoords coords(4, 4, -4, -4);
	GnomonBspline gb(coords);
	plot(gb.get_trunk(), gb.get_trunk_support(), "red");
	plot(gb.get_x_shifted(), gb.get_x_shifted_support(), "navy");
	plot(gb.get_y_shifted(), gb.get_y_shifted_support(), "cyan");
	plot(gb.get_glue(), gb.get_glue_support(), "black");

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

