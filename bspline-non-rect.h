
#include "bspline.h"
#include "linear-combination.h"

#ifndef BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H
#define BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H

class BsplineNonRect : public Bspline {
public:
    BsplineNonRect(const vector<double> &_x_knots, const vector<double> &_y_knots, const vector<double> &_not_defined,
                     double _constant = 1.0):
            Bspline(_x_knots, _y_knots, _constant),
            not_defined(_not_defined) {
    }

    double apply(double x, double y) const;

private:
    // not_defined vector says where BsplineNonRect is equal 0, its length is always 4: {left, up, right, down}
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
    GnomonBspline(double _x_mid, double _y_mid, double _shift_x, double _shift_y):
		GnomonBspline(GnomonBsplineCoords(_x_mid, _y_mid, _shift_x, _shift_y)) {
	}

    GnomonBspline(const GnomonBsplineCoords& c):
			LinearCombination({ &trunk, &x_shifted, &y_shifted, &glue }),
			coords(c),
            trunk(make_trunk(c)),
            x_shifted(make_x_shifted(c)),
			y_shifted(make_y_shifted(c)),
			glue(make_glue(c)) {
    }

	double get_x_mid() const { return coords.x_mid; }
	double get_y_mid() const { return coords.y_mid; }
	double get_shift_x() const { return coords.shift_x; }
	double get_shift_y() const { return coords.shift_y; }

	const BsplineNonRect& get_trunk() const { return trunk; }
	const Bspline& get_x_shifted() const { return x_shifted; }
	const Bspline& get_y_shifted() const { return y_shifted; }
	const LinearCombination& get_glue() const { return glue; }

	Rect get_support() const {
		return get_trunk_support();	
	}

	Rect get_trunk_support() const { 
		return Rect(
			coords.x_from(), coords.x_to(),
			coords.y_from(), coords.y_to()
		);
	}

	Rect get_trunk_support_1() const { 
		return Rect(
			coords.x_from(), coords.x_mid,
			coords.y_from(), coords.y_mid
		);
	}

	Rect get_trunk_support_2() const { 
		return Rect(
			coords.x_mid, coords.x_to(),
			coords.y_from(), coords.y_mid
		);
	}

	Rect get_trunk_support_3() const { 
		return Rect(
			coords.x_from(), coords.x_mid,
			coords.y_mid, coords.y_to()
		);
	}

	Rect get_x_shifted_support() const { return x_shifted.get_support_as_rect(); }

	Rect get_y_shifted_support() const { return y_shifted.get_support_as_rect(); }

	Rect get_glue_support() const {
		return get_glue_support(coords);
	}

private:
	static Rect get_glue_support(const GnomonBsplineCoords& c) {
		return Rect(
			c.x_mid, c.x_pivot(),
			c.y_mid, c.y_pivot()
		);
	}

	static BsplineNonRect make_trunk(const GnomonBsplineCoords& c);
	static Bspline make_x_shifted(const GnomonBsplineCoords& c);
	static Bspline make_y_shifted(const GnomonBsplineCoords& c);
	static LinearCombination make_glue(const GnomonBsplineCoords& c);

	GnomonBsplineCoords coords;
	BsplineNonRect trunk;
	Bspline x_shifted;
	Bspline y_shifted;
	LinearCombination glue;
};

class GnomonNurbs : public Quotient {
public:
    GnomonNurbs(const GnomonBspline& _gnomon_bspline, const Function2D& _divisor):
		Quotient(_gnomon_bspline, _divisor),
		gnomon_bspline(_gnomon_bspline),
		trunk(_gnomon_bspline.get_trunk(), _divisor),
		x_shifted(_gnomon_bspline.get_x_shifted(), _divisor),
		y_shifted(_gnomon_bspline.get_y_shifted(), _divisor),
		glue(_gnomon_bspline.get_glue(), _divisor) {
	}

	const Quotient& get_trunk() const { return trunk; }
	const Quotient& get_x_shifted() const { return x_shifted; }
	const Quotient& get_y_shifted() const { return y_shifted; }
	const Quotient& get_glue() const { return glue; }

	Rect get_support() const {
		return gnomon_bspline.get_support();	
	}

	Rect get_trunk_support() const { 
		return gnomon_bspline.get_trunk_support();
	}

	Rect get_trunk_support_1() const { 
		return gnomon_bspline.get_trunk_support_1();
	}

	Rect get_trunk_support_2() const { 
		return gnomon_bspline.get_trunk_support_2();
	}

	Rect get_trunk_support_3() const { 
		return gnomon_bspline.get_trunk_support_3();
	}

	Rect get_x_shifted_support() const {
		return gnomon_bspline.get_x_shifted_support();
	}

	Rect get_y_shifted_support() const {
		return gnomon_bspline.get_y_shifted_support();
	}

	Rect get_glue_support() const {
		return gnomon_bspline.get_glue_support();
	}

private:
	GnomonBspline gnomon_bspline;
	Quotient trunk, x_shifted, y_shifted, glue;
};

class NurbsOverAdaptedGrid {
public:
	NurbsOverAdaptedGrid(int depth);

	const Function2D& get_bspline(int index) {
		return *scaled_bsplines[index];
	}

	vector<Function2D*> get_all_bsplines() {
		return scaled_bsplines;
	}

	Rect get_bspline_support(int index) {
		Bspline* regular = dynamic_cast<Bspline*>(unscaled_bsplines[index]);
		if (regular != nullptr)
			return regular->get_support_as_rect();
		return dynamic_cast<GnomonBspline*>(unscaled_bsplines[index])->get_support();
	}

private:
	vector<Function2D*> unscaled_bsplines, scaled_bsplines;
};

#endif //BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H
