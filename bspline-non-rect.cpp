
#include <algorithm>
#include <fstream>
#include "bspline-non-rect.h"
#include "bspline.h"
#include "cube.h"

double BsplineNonRect::apply(double x, double y) const {
	double x_min = min(not_defined[0], not_defined[1]);
	double x_max = max(not_defined[0], not_defined[1]);
	double y_min = min(not_defined[2], not_defined[3]);
	double y_max = max(not_defined[2], not_defined[3]);

	if (x_min <= x && x <= x_max && y_min <= y && y <= y_max) {
		return 0;
	} else {
		return Bspline::apply(x, y);
	}
}

BsplineNonRect GnomonBspline::make_trunk(const GnomonBsplineCoords& c) {
	return BsplineNonRect(
			{ c.x_from(), c.x_mid, c.x_mid, c.x_to() },
			{ c.y_from(), c.y_mid, c.y_mid, c.y_to() },
			{ c.x_mid, c.x_to(), c.y_mid, c.y_to() },
			1.0
			);
}

Bspline GnomonBspline::make_x_shifted(const GnomonBsplineCoords& c) {
	return Bspline(
			{ c.x_pivot(), c.x_pivot(), c.x_pivot(), c.x_to() },
			{ c.y_mid, c.y_mid, c.y_mid, c.y_pivot() },
			0.25
			);
}

Bspline GnomonBspline::make_y_shifted(const GnomonBsplineCoords& c) {
	return Bspline(
			{ c.x_mid, c.x_mid, c.x_mid, c.x_pivot() },
			{ c.y_pivot(), c.y_pivot(), c.y_pivot(), c.y_to() },
			0.25
			);
}

LinearCombination GnomonBspline::make_glue(const GnomonBsplineCoords& c) {
	// TODO I know those allocs are evil xD
	Bspline* inner = new Bspline(
			{ c.x_mid, c.x_mid, c.x_mid, c.x_pivot() },
			{ c.y_mid, c.y_mid, c.y_mid, c.y_pivot() },
			0.25
			);

	Bspline* outer = new Bspline(
			{ c.x_mid, c.x_pivot(), c.x_pivot(), c.x_pivot() },
			{ c.y_mid, c.y_pivot(), c.y_pivot(), c.y_pivot() },
			0.25
			);

	LinearFunction* fix = new LinearFunction(
			-1.0 / c.shift_x,
			-1.0 / c.shift_y,
			0.75 + c.x_mid / c.shift_x + c.y_mid / c.shift_y
			);

	return LinearCombination(
			{ inner, outer, new ZeroOutside(fix, get_glue_support(c)) }
			);
}

void read_vector(ifstream& in, vector<double>* out, int cnt) {
	for (int i = 0; i < cnt; i++) {
		double x;
		in >> x;
		out->push_back(x);
	}
}

NurbsOverAdaptedGrid::NurbsOverAdaptedGrid(int depth) {
	string d = to_string(depth);
	string grid_file = "grid-" + d + ".dat";
	string cmd = "./generate --knots " + d + " > " + grid_file;
	system(cmd.c_str());
	ifstream fin(grid_file);

	int N; // number of elements
	fin >> N;
	for (int i = 0; i < N; i++) {
		Bounds b;
		fin >> b.left >> b.right >> b.up >> b.down;
	}

	int M; // number of B-splines
	fin >> M;
	for (int i = 0; i < M; i++) {
		string type;
		// Regular or Gnomon
		fin >> type;
		if (type == "Regular") {
			vector<double> x_knots, y_knots;
			read_vector(fin, &x_knots, 4);
			read_vector(fin, &y_knots, 4);
			Bspline* regular = new Bspline(x_knots, y_knots);
			unscaled_bsplines.push_back(regular);
		} else {  // type == "Gnomon"
			double x_mid, y_mid, shift_x, shift_y;
			fin >> x_mid >> y_mid >> shift_x >> shift_y;
			GnomonBspline* gnomon = new GnomonBspline(x_mid, y_mid, shift_x, shift_y);
			unscaled_bsplines.push_back(gnomon);
		}
	}

	LinearCombination* sum_of_unscaled = new LinearCombination(unscaled_bsplines);
	for (const Function2D* unscaled_bspline: unscaled_bsplines) {
		Quotient* scaled_bspline = new Quotient(unscaled_bspline, sum_of_unscaled);
		scaled_bsplines.push_back(scaled_bspline);
	}
}

