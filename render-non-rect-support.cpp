
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linear-combination.h"
#include "bspline-non-rect.h"

int SIZE = 8; // in each dimension
int SAMPLE_CNT = 40; // in each dimension

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

	/*
	string main_bspline_file = "main_bspline.dat";
	Bspline2DNonRect main_bspline({0, 4, 4, 8}, {0, 4, 4, 8}, {0, 4, 0, 4}, 1.0);
	samples_2d(main_bspline, main_bspline.get_support_as_rect(), main_bspline_file, SAMPLE_CNT);
	print_plot_command(main_bspline_file, "red", false);

	string l_bspline_file = "left_bspline.dat";
	Bspline2D l_bspline({2, 4, 4, 4}, {0, 2, 2, 2}, 0.25);
	samples_2d(l_bspline, l_bspline.get_support_as_rect(), l_bspline_file, SAMPLE_CNT);
	print_plot_command(l_bspline_file, "navy", true);

	string r_bspline_file = "right_bspline.dat";
	Bspline2D r_bspline({0, 2, 2, 2}, {2, 4, 4, 4}, 0.25);
	samples_2d(r_bspline, r_bspline.get_support_as_rect(), r_bspline_file, SAMPLE_CNT);
	print_plot_command(r_bspline_file, "cyan", true);

	string glue_file = "glue.dat";
	Bspline2D inner_glue_bspline({2, 2, 2, 4}, {2, 2, 2, 4}, 0.25);
	Bspline2D outer_glue_bspline({2, 4, 4, 4}, {2, 4, 4, 4}, 0.25);
	LinearFunction linear_fix(0.25, 0.25, -1.25);
	LinearCombination glue({ &inner_glue_bspline, &outer_glue_bspline, &linear_fix });
	samples_2d(glue, Rect2D(2, 4, 2, 4), glue_file, SAMPLE_CNT);
	print_plot_command(glue_file, "black", true);
	*/

	GnomonBsplineCoords coords(4, 4, -4, -4);
	GnomonBspline total(coords);
	string total_file = "total.dat";
	samples_2d(total, Rect2D(0, 8, 0, 8), total_file, SAMPLE_CNT);
	print_plot_command(total_file, "red", false);

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

