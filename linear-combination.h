
#ifndef BSPLINE_SINGULARITIES_GALOIS_LINEARCOMBINATION_H
#define BSPLINE_SINGULARITIES_GALOIS_LINEARCOMBINATION_H

#include "gnuplot.h"
#include "bspline.h"

class LinearCombination : public Function2D {
public:

    LinearCombination(const vector<Function2D*>& _funs):
		Function2D(get_enclosing_cube(_funs)), funs(_funs) {
	}

    double apply(double x, double y) const;

private:

	static Cube get_enclosing_cube(const vector<Function2D*>& funs);

    vector<Function2D*> funs;
};

#endif // BSPLINE_SINGULARITIES_GALOIS_LINEARCOMBINATION_H
