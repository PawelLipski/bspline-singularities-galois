
#include "bspline.h"
#include "gnuplot.h"

double Bspline::apply(double x, double y) const {
	return constant * apply_1d(x_knots, x) * apply_1d(y_knots, y);
}

Cube Bspline::get_containing_cube(const vector<double>& _x_knots, const vector<double>& _y_knots) {
	return Cube(
		_x_knots.front(), _x_knots.back(),
		_y_knots.front(), _y_knots.back());
}

Rect Bspline::get_support_as_rect() const {
	return Rect(
		x_knots.front(), x_knots.back(),
		y_knots.front(), y_knots.back());
}

double Bspline::apply_1d(const vector<double>& knots, double point) {
	int order = knots.size() - 2;

	// 0..order o's, 0..order b's
	double* values = new double[(order+1)*(order+1)];
	#define val(o,b) values[(o) * (order+1) + (b)]

	for (int o = 0; o <= order; o++) {
		for (int b = 0; b <= order-o; b++) {
			if (o == 0) {
				bool point_covered = knots[b] <= point && point < knots[b+1];
				val(0, b) = point_covered ? 1.0 : 0.0;
			} else {
				double left_num  = point - knots[b];
				double left_den  = knots[b+o] - knots[b];
				double right_num = knots[b+o+1] - point;
				double right_den = knots[b+o+1] - knots[b+1];
			
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

