
#include <iostream>
using namespace std;

#include "bspline.h"

struct FunctionDef {
	vector<double> x_knots, y_knots;
	string color;
} function_defs[] = {
	// edged-8
//	{ {0, 0, 4, 4},     {0, 0, 4, 4},     "green" },
//	{ {0, 0, 4, 4},     {4, 4, 6, 8},     "brown" },
//	{ {0, 4, 4, 6},     {10, 12, 12, 16}, "red" },
//	{ {4, 4, 6, 8},     {0, 4, 4, 6},     "navy" },
//	{ {6, 6, 7, 8},     {8, 9, 10, 10},   "purple" },
//	{ {6, 6, 7, 8},     {7, 8, 9, 10},    "dark-blue" },
//	{ {7, 8, 9, 10},    {6, 7, 8, 9},     "magenta" },
//	{ {6, 8, 10, 12},   {12, 12, 16, 16}, "blue" },
//	{ {9, 10, 10, 12},  {10, 10, 12, 12}, "turquoise" },
//	{ {10, 10, 12, 12}, {0, 4, 4, 6},     "dark-green" },
//	{ {10, 12, 12, 16}, {6, 6, 8, 10},    "cyan" },
//	{ {10, 12, 12, 16}, {6, 8, 10, 10},   "cyan" },
//	{ {12, 12, 16, 16}, {10, 12, 12, 16}, "orange" },

	// edged-4
	{ {0, 0, 4, 4},     {0, 0, 4, 4},     "green" },
	{ {0, 4, 4, 6},     {10, 12, 12, 16}, "red" },
	{ {4, 8, 12, 12},   {0, 4, 4, 6},     "navy" },
	{ {7, 8, 9, 10},    {6, 7, 8, 9},     "magenta" },
	{ {6, 8, 10, 12},   {12, 12, 16, 16}, "blue" },
	{ {12, 12, 16, 16}, {8, 12, 12, 16},  "orange" },
	
	// unedged
	{ {0, 8, 12, 16},   {16, 20, 24, 32}, "red" },
	{ {12, 16, 20, 24}, {0, 8, 12, 14},   "navy" },

};

const int function_def_cnt = sizeof(function_defs) / sizeof(function_defs[0]);


int SAMPLE_CNT = 31; // in each dimension

void output_predef_function(int index, const string& data_file) {
	Bspline2D bspline(function_defs[index].x_knots, function_defs[index].y_knots, 1.0);
	samples_2d(&bspline, data_file, SAMPLE_CNT);
}

struct Bounds {
	int left, right, up, down;
};

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (string(argv[1]) == "-s") {
		output = SCREEN;
	}
	int N;
	cin >> N;
	vector<Bounds> bs;
	int size = 0;
	for (int i = 0; i < N; i++) {
		Bounds b;
		cin >> b.left >> b.right >> b.up >> b.down;
		bs.push_back(b);
		size = max(size, max(b.right, b.down));
	}
	for (int i = 0; i < N; i++) {
		const Bounds& b = bs[i];
		int left = b.left, right = b.right, up = b.up, down = b.down;
		if (left == right && up == down)
			continue;  // skip vertices
		bool hl = left == right || up == down;  // highlight double edges
		print_grid_line(left,  up,   right, up,   hl);
		print_grid_line(right, up,   right, down, hl);
		print_grid_line(right, down, left,  down, hl);
		print_grid_line(left,  down, left,  up,   hl);
	}
	print_config(size, SAMPLE_CNT);
	print_rotate_view(60, 45);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	for (int i = 2; i < argc; i++) {
		string data_file = string("bspline") + argv[i] + ".dat";
		int function_index = atoi(argv[i]);
		output_predef_function(function_index, data_file);
		print_plot_command(data_file, function_defs[function_index].color, i > 2);
	}
	cout << endl;
	if (output == SCREEN)
		print_pause();
}

