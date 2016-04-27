//
// Created by Bartosz Janota on 18/04/16.
//

#include "bspline.h"
#include "linear-combination.h"

#ifndef BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H
#define BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H

class Bspline2DNonRect : public Bspline2D {
public:
    Bspline2DNonRect(const vector<double> &_x_knots, const vector<double> &_y_knots, const vector<double> &_not_defined,
                     double _constant = 1.0) :
            Bspline2D(_x_knots, _y_knots, _constant),
            not_defined(_not_defined) {
    }

    double apply(double x, double y) const;

private:

    //not_defined vector says where Bspline2DNonRect is equal 0, its length is always 4: {left, up, right, down}
    vector<double> not_defined;

};

struct GnomonBsplineCoords {
	double x_mid, y_mid, shift_x, shift_y;
	GnomonBsplineCoords(double _x_mid, double _y_mid, double _shift_x, double _shift_y):
		x_mid(_x_mid), y_mid(_y_mid), shift_x(_shift_x), shift_y(_shift_y) {
	}

	double x_from() const {
		return x_mid - shift_x;
	}

	double x_pivot() const {
		return x_mid + shift_x / 2;
	}

	double x_to() const {
		return x_mid + shift_x;
	}

	double y_from() const {
		return y_mid - shift_y;
	}

	double y_pivot() const {
		return y_mid + shift_y / 2;
	}

	double y_to() const {
		return y_mid + shift_y;
	}
};

class GnomonBspline : public LinearCombination {
public:
    GnomonBspline(const GnomonBsplineCoords& c):
			LinearCombination({ &trunk, &x_shifted, &y_shifted, &glue }),
			coords(c),
            trunk(make_trunk(c)),
            x_shifted(make_x_shifted(c)),
			y_shifted(make_y_shifted(c)),
			glue(make_glue(c)) {
    }

	const Bspline2DNonRect& get_trunk() const { return trunk; }
	const Bspline2D& get_x_shifted() const { return x_shifted; }
	const Bspline2D& get_y_shifted() const { return y_shifted; }
	const LinearCombination& get_glue() const { return glue; }

	Rect2D get_trunk_support() const { 
		return Rect2D(
			coords.x_from(), coords.x_to(),
			coords.y_from(), coords.y_to()
		);
	}

	Rect2D get_x_shifted_support() const { return x_shifted.get_support_as_rect(); }

	Rect2D get_y_shifted_support() const { return y_shifted.get_support_as_rect(); }

	Rect2D get_glue_support() const {
		return Rect2D(
			coords.x_mid, coords.x_pivot(),
			coords.y_mid, coords.y_pivot()
		);
	}

private:
	static Bspline2DNonRect make_trunk(const GnomonBsplineCoords& c);
	static Bspline2D make_x_shifted(const GnomonBsplineCoords& c);
	static Bspline2D make_y_shifted(const GnomonBsplineCoords& c);
	static LinearCombination make_glue(const GnomonBsplineCoords& c);

	GnomonBsplineCoords coords;
	Bspline2DNonRect trunk;
	Bspline2D x_shifted;
	Bspline2D y_shifted;
	LinearCombination glue;
};

#endif //BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H
