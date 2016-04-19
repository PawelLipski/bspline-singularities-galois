#ifndef BSPLINE_SINGULARITIES_GALOIS_BSPLINE_H
#define BSPLINE_SINGULARITIES_GALOIS_BSPLINE_H

#include "gnuplot.h"

class Bspline2D: public Function2D {
public:

	Bspline2D(const vector<double> &_x_knots, const vector<double> &_y_knots, double _constant = 1.0) :
			Function2D(get_containing_cube(_x_knots, _y_knots)),
			x_knots(_x_knots),
			y_knots(_y_knots),
			constant(_constant) {
	}

	double apply(double x, double y) const;

	vector<double> get_x_knots() const {
		return x_knots;
	}

	vector<double> get_y_knots() const {
		return y_knots;
	}

	Cube get_containing_cube() const {
		return get_containing_cube(x_knots, y_knots);
	}

private:

	static Cube get_containing_cube(const vector<double>& _x_knots, const vector<double>& _y_knots);

	static double apply_1d(const vector<double>& knots, double point);

	vector<double> x_knots, y_knots;

	double constant;
};

#endif // BSPLINE_SINGULARITIES_GALOIS_BSPLINE_H
