#include <ostream>
#include <iostream>
#include "linear-combination.h"

double LinearCombination::apply(double x, double y) const {
	double result = 0.0;
	for (const Function2D* fun: funs)
		result += fun->apply(x, y);
	return result;
}

