//
// Created by Bartosz Janota on 18/04/16.
//

#include "bsplineNonRect.h"
#include "bspline.h"

double Bspline2DNonRect::apply(double x, double y) const {
    if (x >= not_defined[0] && x <= not_defined[2] && y <= not_defined[1] && y >= not_defined[3]) {
        return 0;
    } else {
        return Bspline2D::apply(x, y);
    }
}