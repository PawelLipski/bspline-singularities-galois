
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
	if (string(argv[1]) == "-s")
		output = SCREEN;

	int size = generate_and_render_grid(3);
	print_config(size, SAMPLE_CNT);
	print_rotate_view(60, 45);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	string type = argv[2]; // Regular or Gnomon
	NurbsOverAdaptedGrid nurbs(3);
	int no = 0;
	for (int i = 3; i < argc; i++) {
		string file = string() + "bspline-" + argv[i] + ".dat";
		int index = atoi(argv[i]);
		Rect support = nurbs.get_bspline_support(index);
		if (type == "Regular") {
			samples_2d(nurbs.get_bspline(index), support, file, SAMPLE_CNT);
			print_plot_command(file, colors[no++], i > 3);
		} else {
			//const GnomonBspline& bspline = dynamic_cast<const GnomonBspline&>(nurbs.get_bspline(index));
			//samples_2d(bspline, support, file, SAMPLE_CNT);
		}
	}
	cout << endl;
	if (output == SCREEN)
		print_pause();
}

