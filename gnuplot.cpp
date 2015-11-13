#include <fstream>
#include <iostream>
#include <vector>

#include "bsplines.cpp"

using namespace std;

const int SIZE = 8;
const int SAMPLES = SIZE * 4 - 1;


/*** B-spline sampling ***/

double bspline(const vector<double>& nodes, double point) {

	int order = nodes.size() - 2;

	// 0..order o's, 0..order b's
	double* values = new double[(order+1)*(order+1)];
	#define val(o,b) values[(o) * (order+1) + (b)]

	for (int o = 0; o <= order; o++) {
		for (int b = 0; b <= order-o; b++) {
			if (o == 0) {
				bool point_covered = nodes[b] <= point && point < nodes[b+1];
				val(0, b) = point_covered ? 1.0 : 0.0;
			} else {
				double left_num  = point - nodes[b];
				double left_den  = nodes[b+o] - nodes[b];
				double right_num = nodes[b+o+1] - point;
				double right_den = nodes[b+o+1] - nodes[b+1];
			
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

void bspline_samples_2d(const vector<double>& x_nodes, const vector<double>& y_nodes, int sample_cnt, const char* data_file) {
	ofstream fout(data_file);
	int interval_cnt = sample_cnt - 1;
	for (int xi = 0; xi < sample_cnt; xi++) {	
		double x = interpolate(x_nodes.front(), x_nodes.back(), xi, interval_cnt);
		double x_val = bspline(x_nodes, x);	

		for (int yi = 0; yi < sample_cnt; yi++) {
			double y = interpolate(y_nodes.front(), y_nodes.back(), yi, interval_cnt);
			double y_val = bspline(y_nodes, y);	
			fout << x << " " << y << " " << x_val * y_val << endl;
		}
	}
	fout.close();
}


/*** Gnuplot script generation ***/

const char* data_file = "bspline.dat";

void print_config() {
	cout << "unset border; unset xtics; unset ytics; unset ztics; set key off" << endl;
	cout << "set hidden3d" << endl;
	cout << "set dgrid3d " << SAMPLES << ", " << SAMPLES << endl;
	cout << "set xrange [0:" << SIZE << "]" << endl;
	cout << "set yrange [0:" << SIZE << "]" << endl;
}

void print_grid_line(int x1, int y1, int x2, int y2) {
	static int line_no = 1;
	cout << "set arrow " << line_no << " from "
			<< x1 << "," << y1 << ",0 to "
			<< x2 << "," << y2 << ",0 nohead" << endl;
	line_no++;
}

void print_all_functions() {
	bspline_samples_2d({0, 2, 2, 4}, {0, 2, 2, 4}, SAMPLES, data_file);
}

void print_splot_command() {
	cout << "splot \"" << data_file << "\" with lines" << endl;  // alternatively: with pm3d
	cout << "pause 10" << endl;
}


int main() {
	int N;
	cin >> N;
	for (int i = 0; i < N; i++) {
		int left, right, up, down;
		cin >> left >> right >> up >> down;
		print_grid_line(left,  up,   right, up);
		print_grid_line(right, up,   right, down);
		print_grid_line(right, down, left,  down);
		print_grid_line(left,  down, left,  up);
	}
	print_config();
	print_all_functions();
	print_splot_command();
}

