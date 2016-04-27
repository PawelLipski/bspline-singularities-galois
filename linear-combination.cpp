
#include "linear-combination.h"

double LinearCombination::apply(double x, double y) const {
	double result = 0.0;
	for (const Function2D* fun: funs)
		result += fun->apply(x, y);
	return result;
}

double ZeroOutside::apply(double x, double y) const {
	if (area.x_from <= x && x <= area.x_to && area.y_from <= y && y <= area.y_to)
		return fun->apply(x, y);
	else
		return 0.0;
}
