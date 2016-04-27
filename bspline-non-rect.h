//
// Created by Bartosz Janota on 18/04/16.
//

#include "bspline.h"

#ifndef BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H
#define BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H


class Bspline2DNonRect : public Bspline2D {
public:
    Bspline2DNonRect(const vector<double> &_x_knots, const vector<double> &_y_knots, const vector<double> &_not_defined,
                     double _constant) :
            Bspline2D(_x_knots, _y_knots, _constant),
            not_defined(_not_defined) {
    }

    double apply(double x, double y) const;

private:

    //not_defined vector says where Bspline2DNonRect is equal 0, its length is always 4: {left, up, right, down}
    vector<double> not_defined;

};


#endif //BSPLINE_SINGULARITIES_GALOIS_BSPLINENONRECT_H
