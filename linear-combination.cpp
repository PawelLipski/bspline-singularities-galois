
#include "linear-combination.h"

double LinearCombination::apply(double x, double y) const {
	double result = 0.0;
	for (unsigned i = 0; i < funs.size(); i++)
		result += funs[i]->apply(x, y) * coefs[i];
	return result;
}

double Quotient::apply(double x, double y) const {
	double divisor_value = divisor.apply(x, y);
	return divisor_value != 0.0 ? dividend.apply(x, y) / divisor_value : 0.0;
}
double ZeroOutside::apply(double x, double y) const {
	if (area.x_from <= x && x <= area.x_to && area.y_from <= y && y <= area.y_to)
		return fun->apply(x, y);
	else
		return 0.0;
}
