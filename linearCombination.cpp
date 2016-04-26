#include <ostream>
#include <iostream>
#include "linearCombination.h"

double LinearCombination::apply(double x, double y) const {
	double result = 0.0;
	for (const Function2D* fun: funs)
		result += fun->apply(x, y);
	return result;
}

Cube LinearCombination::get_enclosing_cube(const vector<Function2D*>& funs) {
	Cube result = funs.front()->get_support();
	for (const Function2D* fun: funs) {
		Cube cube = fun->get_support();
		result = result.get_cube_enclosing_both(cube);
	}
	return result;
}

