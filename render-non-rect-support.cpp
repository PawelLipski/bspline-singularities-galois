
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linear-combination.h"
#include "bspline-non-rect.h"

int SIZE = 8; // in each dimension
int SAMPLE_CNT = 41; // in each dimension

void plot(const Function2D& f, const Rect& area, string color, int tag = 1) {
	static bool replot = false;

	string file = "plot-" + color + "-" + to_string(tag) + ".dat";
	samples_2d(f, area, file, SAMPLE_CNT);
	print_plot_command(file, color, replot);
	replot = true;
}

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (argc == 1 || string(argv[1]) == "-s")
		output = SCREEN;

	print_grid_rect(0, 0, 4, 4, true);
	print_grid_rect(4, 0, 8, 4, true);
	print_grid_rect(4, 4, 8, 8, true);
	print_grid_rect(0, 4, 4, 8, true);

	print_grid_line(0, 2, 4, 2, false);
	print_grid_line(2, 0, 2, 4, false);

	print_config(SIZE, SAMPLE_CNT);
	int angle = 330;
	if (argc >= 3)
		angle = atoi(argv[2]);
	print_rotate_view(30, angle);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	GnomonBsplineCoords coords(4, 4, -4, -4);
	GnomonBspline gb(coords);
	int part = 0;
	if (argc >= 4)
		part = atoi(argv[3]);
	#define P(x) if (part == 0 || part == x || (part < 0 && part <= -x))
	P(1) plot(gb.get_trunk(), gb.get_trunk_support_1(), "red", 1);
	P(1) plot(gb.get_trunk(), gb.get_trunk_support_2(), "red", 2);
	P(1) plot(gb.get_trunk(), gb.get_trunk_support_3(), "red", 3);
	P(2) plot(gb.get_x_shifted(), gb.get_x_shifted_support(), "navy");
	P(3) plot(gb.get_y_shifted(), gb.get_y_shifted_support(), "cyan");
	P(4) plot(gb.get_glue(), gb.get_glue_support(), "black");
	#define Q(x) if (part == x)
	Q(5) plot(gb.get_glue().get_inner(), gb.get_glue_support(), "orange");
	Q(6) plot(gb.get_glue().get_outer(), gb.get_glue_support(), "blue");
	Q(7) plot(gb.get_glue().get_fix(), gb.get_glue_support(), "green");

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

