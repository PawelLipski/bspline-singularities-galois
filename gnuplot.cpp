#include <iostream>
using namespace std;

int line_no = 1;

void print_header() {
	int size = 32;
	cout << "unset border; unset xtics; unset ytics; unset ztics; set key off" << endl;
	cout << "set xrange [0:" << size << "]; set yrange [0:" << size << "]" << endl;
}

void print_grid_line(int x1, int y1, int x2, int y2) {
	cout << "set arrow " << line_no << " from "
			<< x1 << "," << y1 << ",0 to "
			<< x2 << "," << y2 << ",0 nohead" << endl;
	line_no++;
}

void print_footer() {
	cout << "splot 256**2-(abs(x-16)*abs(y-16))**2; pause 10" << endl;
}

int main() {
	int N;
	cin >> N;
	print_header();
	for (int i = 0; i < N; i++) {
		int left, right, up, down;
		cin >> left >> right >> up >> down;
		print_grid_line(left,  up,   right, up);
		print_grid_line(right, up,   right, down);
		print_grid_line(right, down, left,  down);
		print_grid_line(left,  down, left,  up);
	}
	print_footer();
}

