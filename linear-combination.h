
#ifndef BSPLINE_SINGULARITIES_GALOIS_LINEARCOMBINATION_H
#define BSPLINE_SINGULARITIES_GALOIS_LINEARCOMBINATION_H

#include "gnuplot.h"

class LinearFunction: public Function2D {
public:
	LinearFunction(double _a, double _b, double _c):
		a(_a), b(_b), c(_c) {
	}

	double apply(double x, double y) const {
		return a * x + b * y + c;
	}

private:
	double a, b, c;
};

class ZeroOutside: public Function2D {
public:
	ZeroOutside(const Function2D* _fun, const Rect& _area):
		fun(_fun), area(_area) {
	}

	double apply(double x, double y) const;

private:
	const Function2D* fun;
	Rect area;
};

class Quotient: public Function2D {
public:
	Quotient(const Function2D& _dividend, const Function2D& _divisor):
		dividend(_dividend), divisor(_divisor) {
	}

	double apply(double x, double y) const;

protected:
	const Function2D &dividend, &divisor;
};

class LinearCombination : public Function2D {
public:
    LinearCombination(const vector<Function2D*>& _funs):
		funs(_funs), coefs(_funs.size(), 1.0) {
	}

    LinearCombination(const vector<Function2D*>& _funs, const vector<double>& _coefs):
		funs(_funs), coefs(_coefs) {
	}

    double apply(double x, double y) const;

private:
    vector<Function2D*> funs;
	vector<double> coefs;
};

#endif // BSPLINE_SINGULARITIES_GALOIS_LINEARCOMBINATION_H
