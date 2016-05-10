
#include <climits>
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linear-combination.h"
#include "bspline-non-rect.h"

int SAMPLE_CNT = 31; // in each dimension

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (argc == 1 || string(argv[1]) == "-s") 
		output = SCREEN;

	int size = generate_and_render_grid(3);

	print_config(size, SAMPLE_CNT);
	print_rotate_view(30, 45);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	NurbsOverAdaptedGrid nurbs(3);
	LinearCombination sum_of_scaled(nurbs.get_all_bsplines());
	string sum_file = "bspline_sum.dat";
	Rect support(0, size, 0, size);
	samples_2d(sum_of_scaled, support, sum_file, SAMPLE_CNT);
	print_plot_command(sum_file, "red", false);

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

