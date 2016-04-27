//
// Created by Bartosz Janota on 18/04/16.
//

#include <algorithm>
#include "bspline-non-rect.h"
#include "bspline.h"
#include "cube.h"

double Bspline2DNonRect::apply(double x, double y) const {
	double x_min = min(not_defined[0], not_defined[1]);
	double x_max = max(not_defined[0], not_defined[1]);
	double y_min = min(not_defined[2], not_defined[3]);
	double y_max = max(not_defined[2], not_defined[3]);

    if (x_min <= x && x <= x_max && y_min <= y && y <= y_max) {
        return 0;
    } else {
        return Bspline2D::apply(x, y);
    }
}

Bspline2DNonRect GnomonBspline::make_trunk(const GnomonBsplineCoords& c) {
	return Bspline2DNonRect(
		{ c.x_from(), c.x_mid, c.x_mid, c.x_to() },
		{ c.y_from(), c.y_mid, c.y_mid, c.y_to() },
		{ c.x_mid, c.x_to(), c.y_mid, c.y_to() },
		1.0
	);
}

Bspline2D GnomonBspline::make_x_shifted(const GnomonBsplineCoords& c) {
	return Bspline2D(
		{ c.x_pivot(), c.x_to(), c.x_to(), c.x_to() },
		{ c.y_mid, c.y_pivot(), c.y_pivot(), c.y_pivot() },
		0.25
	);
}

Bspline2D GnomonBspline::make_y_shifted(const GnomonBsplineCoords& c) {
	return Bspline2D(
		{ c.x_mid, c.x_pivot(), c.x_pivot(), c.x_pivot() },
		{ c.y_pivot(), c.y_to(), c.y_to(), c.y_to() },
		0.25
	);
}

LinearCombination GnomonBspline::make_glue(const GnomonBsplineCoords& c) {
	// TODO I know those allocs are evil xD
	Bspline2D* inner = new Bspline2D(
		{ c.x_mid, c.x_mid, c.x_mid, c.x_pivot() },
		{ c.y_mid, c.y_mid, c.y_mid, c.y_pivot() },
		0.25
	);

	Bspline2D* outer = new Bspline2D(
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
		{ inner, outer, fix }
	);
}

