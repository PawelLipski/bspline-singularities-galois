#include <fstream>
#include <iostream>
using namespace std;

int line_no = 1;

void print_config() {
	int size = 32;
	cout << "unset border; unset xtics; unset ytics; unset ztics; set key off" << endl;
	//cout << "set dgrid3d " << size << "," << size << endl;
	cout << "set isosamples 50" << endl;
	cout << "set xrange [0:" << size << "]; set yrange [0:" << size << "]" << endl;
	//cout << "set pm3d interpolate 0,0" << endl;
}

void print_grid_line(int x1, int y1, int x2, int y2) {
	cout << "set arrow " << line_no << " from "
			<< x1 << "," << y1 << ",0 to "
			<< x2 << "," << y2 << ",0 nohead" << endl;
	line_no++;
}

double interpolate(double from, double to, int index, int interval_cnt) {
	return from + (to - from) / interval_cnt * index;
}

double function(double x, double y) {
	return x * y;
}

void sample_region(int l, int r, int u, int d, int sample_cnt, const char* data_file) {
	int interval_cnt = sample_cnt - 1;
	ofstream fout(data_file);
	for (int xi = 0; xi < sample_cnt; xi++) {
		double x = interpolate(l, r, xi, interval_cnt);
		for (int yi = 0; yi < sample_cnt; yi++) {
			double y = interpolate(u, d, yi, interval_cnt);
			fout << x << " " << y << " " << function(x, y) << endl;
		}
	}
	fout.close();
}

int function_no = 1;

void print_placeholder_function(int l, int r, int u, int d) {
	cout << "empty(x,y) = "
			<< l << " <= x && x <= " << r << " && "
			<< u << " <= y && y <= " << d << " ? 0 : 1/0" << endl;
}

void print_function_over(int l, int r, int u, int d) {
	cout << "f" << function_no << "(x,y) = "
			<< l << " <= x && x <= " << r << " && "
			<< u << " <= y && y <= " << d << " ? x * (16 - x) * y * (16 - y) : empty(x,y)" << endl;
	function_no++;
}

void print_function_defs() {
	print_placeholder_function(0, 16, 0, 16);
	print_function_over(0, 8, 0, 8);
	print_function_over(0, 8, 8, 16);
	print_function_over(8, 16, 0, 8);
	print_function_over(8, 16, 8, 16);
}

string sum_of_functions(int cnt) {
	string res = "f1(x,y)";
	for (int i = 2; i <= cnt; i++)
		res += " + f" + to_string(i) + "(x,y)";
	return res;
}

void print_splot_command() {
	cout << "splot " << sum_of_functions(4) << " with lines; pause 10" << endl;
}

int main() {
	int N;
	cin >> N;
	print_config();
	for (int i = 0; i < N; i++) {
		int left, right, up, down;
		cin >> left >> right >> up >> down;
		print_grid_line(left,  up,   right, up);
		print_grid_line(right, up,   right, down);
		print_grid_line(right, down, left,  down);
		print_grid_line(left,  down, left,  up);
	}
	print_function_defs();
	print_splot_command();
}

