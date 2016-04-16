//
// Created by Bartosz Janota on 16/04/16.
//

#ifndef BSPLINE_SINGULARITIES_GALOIS_BSPLINE2DLINEARCOMBINATION_H
#define BSPLINE_SINGULARITIES_GALOIS_BSPLINE2DLINEARCOMBINATION_H

#include "gnuplot.h"
#include "bspline.h"

class Bspline2DLinearCombination : public Function2D {
public:

    Bspline2DLinearCombination(const Bspline2D &_b_bspline, const Bspline2D &_b_l_bspline,
                               const Bspline2D &_b_r_bspline, double _constant) :
            Function2D(_b_bspline.get_support()),
            b_bspline(_b_bspline),
            l_bspline(_b_l_bspline),
            r_bspline(_b_r_bspline),
            constant(_constant) {
    }

    double apply(double x, double y) const;

private:

    Bspline2D b_bspline;
    Bspline2D l_bspline;
    Bspline2D r_bspline;

    double constant;
};


#endif //BSPLINE_SINGULARITIES_GALOIS_BSPLINE2DLINEARCOMBINATION_H
