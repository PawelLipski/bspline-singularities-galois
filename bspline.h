#ifndef BSPLINE_SINGULARITIES_GALOIS_BSPLINE_H
#define BSPLINE_SINGULARITIES_GALOIS_BSPLINE_H

#include <algorithm>
#include "gnuplot.h"

static vector<double> asc(vector<double> v) {
	if (!is_sorted(v.begin(), v.end()))
		reverse(v.begin(), v.end());
	return v;
}

class Bspline2D: public Function2D {
public:

	Bspline2D(const vector<double> &_x_knots, const vector<double> &_y_knots, double _constant = 1.0):
			x_knots(asc(_x_knots)),
			y_knots(asc(_y_knots)),
			constant(_constant) {
	}

	double apply(double x, double y) const;

	vector<double> get_x_knots() const {
		return x_knots;
	}

	vector<double> get_y_knots() const {
		return y_knots;
	}

	Rect2D get_support_as_rect() const;

	Cube get_support() const {
		return get_containing_cube(x_knots, y_knots);
	}

private:

	static Cube get_containing_cube(const vector<double>& _x_knots, const vector<double>& _y_knots);

	static double apply_1d(const vector<double>& knots, double point);

	vector<double> x_knots, y_knots;

	double constant;
};

#endif // BSPLINE_SINGULARITIES_GALOIS_BSPLINE_H
