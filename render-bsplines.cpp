
#include <iostream>
using namespace std;

#include "bspline.h"
#include "bspline-non-rect.h"
#include "gnuplot.h"

int SAMPLE_CNT = 31; // in each dimension

string colors[] = { "turquoise", "green", "red", "navy", "magenta", "blue", "orange" };

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (argc == 1 || string(argv[1]) == "-s")
		output = SCREEN;

	int size = generate_and_render_grid(3);
	print_config(size, SAMPLE_CNT);
	print_rotate_view(60, 45);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	NurbsOverAdaptedGrid nurbs(3);
	int no = 0;
	for (int i = 2; i < argc; i++) {
		string file = string() + "bspline-" + argv[i] + ".dat";
		int index = atoi(argv[i]);
		samples_2d(nurbs.get_bspline(index), nurbs.get_bspline_support(index), file, SAMPLE_CNT);
		print_plot_command(file, colors[no++], i > 2);
	}
	cout << endl;
	if (output == SCREEN)
		print_pause();
}

