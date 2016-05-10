#include <fstream>
#include <iostream>
#include <sstream>

#include "gnuplot.h"

using namespace std;


/*** Function sampling ***/

double interpolate(double from, double to, int index, int interval_cnt) {
	return from + (to - from) / interval_cnt * index;
}

double samples_2d(const Function2D& f, const Rect& area, const string &data_file, int sample_cnt) {
	double max = 0.0;
	ofstream fout(data_file);
	int interval_cnt = sample_cnt - 1;
	//cerr << "samples2d: " << support.left() << " " << support.right() << " " << support.up() << " " << support.down() << endl;
	for (int xi = 0; xi < sample_cnt; xi++) {
		double x = interpolate(area.x_from, area.x_to, xi, interval_cnt);
		for (int yi = 0; yi < sample_cnt; yi++) {
			double y = interpolate(area.y_from, area.y_to, yi, interval_cnt);

			double val = f.apply(x, y);
			if (val > max) max = val;
			fout << x << " " << y << " " << val << endl;
			//cerr << x << " " << y << " " << val << endl;
		}
	}
	fout.close();
	return max;
}


/*** Gnuplot script generation ***/

void print_config(int size, int sample_cnt) {
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

	cout << "set hidden3d nooffset" << endl;
	cout << "set dgrid3d " << sample_cnt << ", " << sample_cnt << endl;
	cout << "set object 1 rectangle from screen 0,0 to screen 1,1 fillcolor rgb \"white\" behind" << endl;
}

void print_rotate_view(int x, int y) {
	cout << "set view " << x << "," << y << endl;
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

void print_grid_rect(int left, int up, int right, int down, bool highlight) {
	print_grid_line(left,  up,   right, up,   highlight);
	print_grid_line(right, up,   right, down, highlight);
	print_grid_line(right, down, left,  down, highlight);
	print_grid_line(left,  down, left,  up,   highlight);
}

void generate_and_render_grid(int depth) {
	string d = to_string(depth);
	string grid_file = "grid-" + d + ".dat";
	string cmd = "./generate --draw-plain " + d + " > " + grid_file;
	system(cmd.c_str());
	ifstream fin(grid_file);

	int N; // number of elements
	fin >> N;
	vector<Bounds> bs;
	int size = 0;
	for (int i = 0; i < N; i++) {
		Bounds b;
		fin >> b.left >> b.right >> b.up >> b.down;
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
}

void print_plot_command(const string& data_file, const string& color, bool replot) {
	// alternatively: with pm3d
	cout << (replot ? ", " : "splot ") << "\"" << data_file << "\" with lines lc rgb '" << color << "'";
}

void print_pause() {
	cout << "pause 15" << endl;
}

