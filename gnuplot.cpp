#include <fstream>
#include <iostream>
using namespace std;

int line_no = 1;

void print_config() {
	int size = 32;
	cout << "unset border; unset xtics; unset ytics; unset ztics; set key off" << endl;
	cout << "set isosamples 50" << endl;
	cout << "set xrange [0:" << size << "]; set yrange [0:" << size << "]" << endl;
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

int function_no = 1;

void print_function(const string& name, const string& expr_if_defined, const string& expr_if_undefined, int l, int r, int u, int d) {
	cout << name << "(x,y) = "
			<< l << " <= x && x <= " << r << " && "
			<< u << " <= y && y <= " << d << " ? " << expr_if_defined << " : " << expr_if_undefined << endl;
}

void print_placeholder_function(int l, int r, int u, int d) {
	print_function("empty", "0", "1/0", l, r, u, d);
}

void print_numbered_function(int l, int r, int u, int d) {
	print_function("f" + to_string(function_no), "x * (16 - x) * y * (16 - y)", "empty(x,y)", l, r, u, d);
	function_no++;
}

void print_all_functions() {
	print_placeholder_function(0, 16, 0, 16);
	print_numbered_function(0, 8, 0, 8);
	print_numbered_function(0, 8, 8, 16);
	print_numbered_function(8, 16, 0, 8);
	print_numbered_function(8, 16, 8, 16);
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
	print_all_functions();
	print_splot_command();
}

