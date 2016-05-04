
#include <climits>
#include <iostream>
using namespace std;

#include "gnuplot.h"
#include "bspline.h"
#include "linear-combination.h"
#include "bspline-non-rect.h"

int SAMPLE_CNT = 63; // in each dimension

struct Bounds {
	int left, right, up, down;
};

void read_vector(vector<double>* out, int cnt) {
	for (int i = 0; i < cnt; i++) {
		double x;
		cin >> x;
		out->push_back(x);
	}
}

double get_coef(double** B, int row, int col) {
	return B[row][col];
}

void set_coef(double** B, int row, int col, double value) {
	B[row][col] = value;
}

void subtract_row(double** B, double* L, int N, int pivot, int from) {

	double ratio = get_coef(B, from, pivot) / get_coef(B, pivot, pivot); 

	set_coef(B, from, pivot, 0.0);
	for (int col = pivot + 1; col <= N; col++) {
		double orig = get_coef(B, from, col);
		double factor = get_coef(B, pivot, col);
		set_coef(B, from, col, orig - factor * ratio);
	}
	L[from] -= L[pivot] * ratio;
}

void extract_single_unknown(double** B, double* L, double* U, int N, int row) {

	double result = L[row];
	for (int col = row + 1; col < N; col++)
		result -= get_coef(B, row, col) * U[col];
	result /= get_coef(B, row, row);

	U[row] = result;
}

void gauss_solve(double** B, double* L, double* U, int N) {

	for (int pivot = 0; pivot < N-1; pivot++) {
		for (int from = pivot + 1; from < N; from++)
			subtract_row(B, L, N, pivot, from);
	}

	for (int row = N-1; row >= 0; row--)
		extract_single_unknown(B, L, U, N, row);
}

int main(int argc, char** argv) {
	enum OutputTerminal {
		EPS,
		SCREEN
	} output = EPS;
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " -s or " << argv[0] << " <file>" << endl;
		exit(2);
	}
	if (string(argv[1]) == "-s") {
		output = SCREEN;
	}
	int N; // number of elements
	cin >> N;
	vector<Bounds> bs;
	int size = 0;
	for (int i = 0; i < N; i++) {
		Bounds b;
		cin >> b.left >> b.right >> b.up >> b.down;
		bs.push_back(b);
		size = max(size, max(b.right, b.down));
	}
	int x_from = INT_MAX, x_to = 0, y_from = INT_MAX, y_to = 0;
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
		x_from = min(x_from, left);
		x_to = max(x_to, right);
		y_from = min(y_from, up);
		y_to = max(y_to, down);
	}
	print_config(size, SAMPLE_CNT);
	print_rotate_view(30, 45);
	if (output == EPS)
		print_eps_terminal(argv[1]);

	int M; // number of B-splines
	cin >> M;
	vector<Function2D*> bsplines;
	for (int i = 0; i < M; i++) {
		string type; 
		// Regular or Gnomon
		cin >> type;
		if (type == "Regular") {
			vector<double> x_knots, y_knots;
			read_vector(&x_knots, 4);
			read_vector(&y_knots, 4);
			Bspline* regular = new Bspline(x_knots, y_knots);
			bsplines.push_back(regular);
		} else {  // type == "Gnomon"
			double x_mid, y_mid, shift_x, shift_y;
			cin >> x_mid >> y_mid >> shift_x >> shift_y;
			GnomonBspline* gnomon = new GnomonBspline(x_mid, y_mid, shift_x, shift_y);
			bsplines.push_back(gnomon);
		}
	}

	double** B = new double*[N];
	for (int i = 0; i < N; i++)
		B[i] = new double[M];
	// M == N, always
	for (int i = 0; i < N; i++) {
		const Bounds& b = bs[i];
		double x = (b.left + b.right) / 2.0;
		double y = (b.up + b.down) / 2.0;
		for (int j = 0; j < M; j++)
			B[i][j] = bsplines[j]->apply(x, y);		
	}

	double* L = new double[N];
	for (int i = 0; i < N; i++)
		L[i] = 1.0;

	double* U = new double[N];
	gauss_solve(B, L, U, N);

	//for (int i = 0; i < N; i++)
	//	cout << U[i] << endl;
	vector<double> coefs(U, U+N);

	string sum_file = "bspline_sum.dat";
	LinearCombination sum_of_all(bsplines, coefs);
	Rect support(x_from, x_to, y_from, y_to);
	samples_2d(sum_of_all, support, sum_file, SAMPLE_CNT);
	print_plot_command(sum_file, "red", false);

	cout << endl;
	if (output == SCREEN)
		print_pause();
}

