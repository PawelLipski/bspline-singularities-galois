//
// Created by Bartosz Janota on 18/04/16.
//

#include "bspline-non-rect.h"
#include "bspline.h"

double Bspline2DNonRect::apply(double x, double y) const {
    if (not_defined[0] <= x && x <= not_defined[1] && not_defined[2] <= y && y <= not_defined[3]) {
        return 0;
    } else {
        return Bspline2D::apply(x, y);
    }
}
