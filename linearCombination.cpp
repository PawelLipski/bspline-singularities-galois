//
// Created by Bartosz Janota on 16/04/16.
//

#include "linearCombination.h"

double Bspline2DLinearCombination::apply(double x, double y) const {
    return b_bspline.apply(x, y) + l_bspline.apply(x, y) + r_bspline.apply(x, y);
}
