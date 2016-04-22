#include <ostream>
#include <iostream>
#include "linearCombination.h"

double Bspline2DLinearCombination::apply(double x, double y) const {
	double result = 0.0;
	for (const auto& bspline: bsplines)
		result += bspline.apply(x, y);
	double d = 3 / 5.0 *
			   (3 - (1 / 2.0 * x)) * ((1 / 2.0 * y) - 1);
	//cerr << x << " " << y << ": " << d << endl;
	return result + d;
	//return result;
}

Cube Bspline2DLinearCombination::get_enclosing_cube(const vector<Bspline2D>& bsplines) {
	Cube result = bsplines.front().get_containing_cube();
	for (const auto& bspline: bsplines) {
		Cube cube = bspline.get_containing_cube();
		result = result.get_cube_enclosing_both(cube);
	}
	return result;
}

