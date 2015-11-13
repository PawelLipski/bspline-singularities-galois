#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const int SIZE = 8;
const int SAMPLES = SIZE * 4 - 1;
const char* data_file = "bspline.dat";

struct DomainDef {
	vector<double> x_nodes, y_nodes;
} domain_defs[] = {
	// depth = 1
	{ {0, 2, 2, 4}, {0, 2, 2, 4} }, // 4x4
	{ {2, 2, 6, 6}, {2, 2, 6, 6} }, // 4x4
	{ {0, 2, 2, 4}, {2, 4, 6, 6} }, // 4x4

	{ {0, 0, 2, 2}, {0, 2, 2, 4} }, // 2x4
	{ {0, 0, 2, 2}, {2, 4, 6, 6} }, // 4x2

	{ {0, 0, 2, 2}, {0, 0, 2, 2} }, // 2x2

	// depth = 2
	{ {2, 3, 4, 5}, {2, 3, 4, 5} }, // 3x3
};

const int domain_def_cnt = sizeof(domain_defs) / sizeof(domain_defs[0]);


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

void bspline_samples_2d(const vector<double>& x_nodes, const vector<double>& y_nodes) {
	ofstream fout(data_file);
	int interval_cnt = SAMPLES - 1;
	for (int xi = 0; xi < SAMPLES; xi++) {	
		double x = interpolate(x_nodes.front(), x_nodes.back(), xi, interval_cnt);
		double x_val = bspline(x_nodes, x);	

		for (int yi = 0; yi < SAMPLES; yi++) {
			double y = interpolate(y_nodes.front(), y_nodes.back(), yi, interval_cnt);
			double y_val = bspline(y_nodes, y);	
			fout << x << " " << y << " " << x_val * y_val << endl;
		}
	}
	fout.close();
}

void bspline_samples_2d(const vector<double>& nodes) {
	bspline_samples_2d(nodes, nodes);
}


/*** Gnuplot script generation ***/

void print_config(int output_id) {
	cout << "unset border" << endl;
	cout << "set key off" << endl;
	cout << "unset xtics" << endl;
	cout << "unset ytics" << endl;
	cout << "unset ztics" << endl;
	//cout << "set xlabel 'x'" << endl;
	//cout << "set ylabel 'y'" << endl;
	//cout << "set zlabel 'z'" << endl;
	
	cout << "set xrange [0:" << SIZE << "]" << endl;
	cout << "set yrange [0:" << SIZE << "]" << endl;

	cout << "set hidden3d" << endl;
	cout << "set dgrid3d " << SAMPLES << ", " << SAMPLES << endl;
	cout << "set view 60,75" << endl;

	cout << "set terminal eps" << endl;
	cout << "set output \"eps/bspline-" << output_id << ".eps\"" << endl;
}

void print_grid_line(int x1, int y1, int x2, int y2, bool highlight) {
	static int line_no = 1;
	int line_width = highlight ? 5 : 2;
	cout << "set arrow " << line_no << " from "
			<< x1 << "," << y1 << ",0 to "
			<< x2 << "," << y2 << ",0 nohead lc rgb \"gray\" lw " << line_width << endl;
	line_no++;
}

void print_predef_function(int index) {
	bspline_samples_2d(domain_defs[index].x_nodes, domain_defs[index].y_nodes);
}

void print_splot_command() {
	cout << "splot \"" << data_file << "\" with lines" << endl;  // alternatively: with pm3d
	//cout << "pause 10" << endl;
}


int main(int argc, char** argv) {
	int function_index = 0;
	if (argc == 2) {
		function_index = atoi(argv[1]);
		if (function_index >= domain_def_cnt) {
			cerr << "Allowed index range: 0..." << domain_def_cnt-1 << ", inclusively" << endl;
			exit(1);
		}
	}

	int N;
	cin >> N;
	for (int i = 0; i < N; i++) {
		int left, right, up, down;
		cin >> left >> right >> up >> down;
		if (left == right && up == down)
			continue;  // skip vertices
		bool hl = left == right || up == down;  // highlight double edges
		print_grid_line(left,  up,   right, up,   hl);
		print_grid_line(right, up,   right, down, hl);
		print_grid_line(right, down, left,  down, hl);
		print_grid_line(left,  down, left,  up,   hl);
	}
	print_config(function_index);
	print_predef_function(function_index);
	print_splot_command();
}

