
#include <iostream>
using namespace std;

#include "bspline.h"
#include "bspline-non-rect.h"
#include "gnuplot.h"

int SAMPLE_CNT = 41; // in each dimension

int hardcoded_colors[] = { 1, 9, 18, 21, 36, 41, 44 };
string colors[] = { "turquoise", "green", "red", "navy", "magenta", "blue", "orange" };

int find_hardcoded_color(int index) {
	for (unsigned i = 0; i < sizeof(hardcoded_colors) / sizeof(int); i++)
		if (hardcoded_colors[i] == index)
			return i;
	return 0;
}

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
	if (string(argv[1]) == "-s")
		output = SCREEN;

	int size = generate_and_render_grid(3);
	print_config(size, SAMPLE_CNT);
	print_rotate_view(60, atoi(argv[2]));
	if (output == EPS)
		print_eps_terminal(argv[1]);

	string type = argv[3]; // Regular or Gnomon
	NurbsOverAdaptedGrid nurbs(3);
	for (int i = 4; i < argc; i++) {
		string file = string() + "bspline-" + argv[i] + ".dat";
		int index = atoi(argv[i]);
		Rect support = nurbs.get_bspline_support(index);
		if (type == "Regular") {
			samples_2d(nurbs.get_bspline(index), support, file, SAMPLE_CNT);
			int color_index = find_hardcoded_color(index);
			print_plot_command(file, colors[color_index], i > 4);
		} else {
			const GnomonNurbs& gb = dynamic_cast<const GnomonNurbs&>(nurbs.get_bspline(index));
			plot(gb.get_trunk(), gb.get_trunk_support_1(), "red", 1);
			plot(gb.get_trunk(), gb.get_trunk_support_2(), "red", 2);
			plot(gb.get_trunk(), gb.get_trunk_support_3(), "red", 3);
			plot(gb.get_x_shifted(), gb.get_x_shifted_support(), "navy");
			plot(gb.get_y_shifted(), gb.get_y_shifted_support(), "cyan");
			plot(gb.get_glue(), gb.get_glue_support(), "black");
		}
	}
	cout << endl;
	if (output == SCREEN)
		print_pause();
}

