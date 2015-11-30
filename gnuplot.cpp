#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const int SAMPLES = 31;

struct FunctionDef {
	vector<double> x_knots, y_knots;
	string color;
} function_defs[] = {
	// edged-8
	{ {0, 0, 4, 4},     {0, 0, 4, 4},     "green" },
	{ {0, 0, 4, 4},     {4, 4, 6, 8},     "brown" },
	{ {0, 4, 4, 6},     {10, 12, 12, 16}, "red" },
	{ {4, 4, 6, 8},     {0, 4, 4, 6},     "navy" },
	{ {6, 6, 7, 8},     {8, 9, 10, 10},   "purple" },
	{ {6, 6, 7, 8},     {7, 8, 9, 10},    "dark-blue" },
	{ {7, 8, 9, 10},    {6, 7, 8, 9},     "magenta" },
	{ {6, 8, 10, 12},   {12, 12, 16, 16}, "blue" },
	{ {9, 10, 10, 12},  {10, 10, 12, 12}, "turquoise" },
	{ {10, 10, 12, 12}, {0, 4, 4, 6},     "dark-green" },
	{ {10, 12, 12, 16}, {6, 6, 8, 10},    "cyan" },
	{ {10, 12, 12, 16}, {6, 8, 10, 10},   "cyan" },
	{ {12, 12, 16, 16}, {10, 12, 12, 16}, "orange" },

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

int size;

/*** B-spline sampling ***/

double bspline(const vector<double>& knots, double point) {

	int order = knots.size() - 2;

	// 0..order o's, 0..order b's
	double* values = new double[(order+1)*(order+1)];
	#define val(o,b) values[(o) * (order+1) + (b)]

	for (int o = 0; o <= order; o++) {
		for (int b = 0; b <= order-o; b++) {
			if (o == 0) {
				bool point_covered = knots[b] <= point && point < knots[b+1];
				val(0, b) = point_covered ? 1.0 : 0.0;
			} else {
				double left_num  = point - knots[b];
				double left_den  = knots[b+o] - knots[b];
				double right_num = knots[b+o+1] - point;
				double right_den = knots[b+o+1] - knots[b+1];
			
				double left = 0.0, right = 0.0;
				if (left_den != 0.0)
					left = left_num  / left_den * val(o-1, b);
				if (right_den != 0.0)
					right = right_num / right_den * val(o-1, b+1);

				val(o, b) = left + right;
			}
		}
	}

	double result = val(order, 0);
	delete values;
	return result;
}

double interpolate(double from, double to, int index, int interval_cnt) {
	return from + (to - from) / interval_cnt * index;
}

void bspline_samples_2d(const string& data_file, const vector<double>& x_knots, const vector<double>& y_knots) {
	ofstream fout(data_file);
	int interval_cnt = SAMPLES - 1;
	for (int xi = 0; xi < SAMPLES; xi++) {	
		double x = interpolate(x_knots.front(), x_knots.back(), xi, interval_cnt);
		double x_val = bspline(x_knots, x);	

		for (int yi = 0; yi < SAMPLES; yi++) {
			double y = interpolate(y_knots.front(), y_knots.back(), yi, interval_cnt);
			double y_val = bspline(y_knots, y);	
			fout << x << " " << y << " " << x_val * y_val << endl;
		}
	}
	fout.close();
}


/*** Gnuplot script generation ***/

void print_config() {
	cout << "unset border" << endl;
	cout << "set key off" << endl;
	cout << "unset xtics" << endl;
	cout << "unset ytics" << endl;
	cout << "unset ztics" << endl;
	//cout << "set xlabel 'x'" << endl;
	//cout << "set ylabel 'y'" << endl;
	//cout << "set zlabel 'z'" << endl;
	
	cout << "set xrange [0:" << size << "]" << endl;
	cout << "set yrange [0:" << size << "]" << endl;

	cout << "set hidden3d" << endl;
	cout << "set dgrid3d " << SAMPLES << ", " << SAMPLES << endl;
	cout << "set view 60,45" << endl;
}

void print_eps_terminal(const string& output_eps) {
	cout << "set terminal eps" << endl;
	cout << "set output \"eps/" << output_eps << ".eps\"" << endl;
}

void print_grid_line(int x1, int y1, int x2, int y2, bool highlight) {
	static int line_no = 1;
	int line_width = highlight ? 5 : 2;
	cout << "set arrow " << line_no << " from "
			<< x1 << "," << y1 << ",0 to "
			<< x2 << "," << y2 << ",0 nohead lc rgb \"gray\" lw " << line_width << endl;
	line_no++;
}

void output_predef_function(const string& data_file, int index) {
	bspline_samples_2d(data_file, function_defs[index].x_knots, function_defs[index].y_knots);
}

void print_plot_command(const string& data_file, const string& color, bool replot) {
	// alternatively: with pm3d
	cout << (replot ? ", " : "splot ") << "\"" << data_file << "\" with lines lc rgb \"" << color << "\"";
}

void print_pause() {
	cout << "pause 15" << endl;
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
	print_config();
	if (output == EPS)
		print_eps_terminal(argv[1]);

	for (int i = 2; i < argc; i++) {
		string data_file = string("bspline") + argv[i] + ".dat";
		int function_index = atoi(argv[i]);
		output_predef_function(data_file, function_index);
		print_plot_command(data_file, function_defs[function_index].color, i > 2);
	}
	cout << endl;
	if (output == SCREEN)
		print_pause();
}

