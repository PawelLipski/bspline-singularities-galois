
#ifndef BSPLINE_SINGULARITIES_GALOIS_BSPLINE2DLINEARCOMBINATION_H
#define BSPLINE_SINGULARITIES_GALOIS_BSPLINE2DLINEARCOMBINATION_H

#include "gnuplot.h"
#include "bspline.h"

class Bspline2DLinearCombination : public Function2D {
public:

    Bspline2DLinearCombination(const vector<Bspline2D>& _bsplines):
		Function2D(get_enclosing_cube(_bsplines)), bsplines(_bsplines) {
	}

    double apply(double x, double y) const;

private:

	static Cube get_enclosing_cube(const vector<Bspline2D>& bsplines);

    vector<Bspline2D> bsplines;
};

#endif //BSPLINE_SINGULARITIES_GALOIS_BSPLINE2DLINEARCOMBINATION_H
