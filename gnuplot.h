#ifndef BSPLINE_SINGULARITIES_GALOIS_GNUPLOT_H
#define BSPLINE_SINGULARITIES_GALOIS_GNUPLOT_H

#include <string>
#include <vector>

#include "cube.h"

/*** Function sampling ***/

class Function2D {

public:

	Function2D(const Cube& _support): support(_support) {}

	virtual double apply(double x, double y) const = 0;

	const Cube& get_support() const {
		return support;
	}

private:
	Cube support;
};


void samples_2d(const Function2D* f, const string& data_file, int sample_cnt);


/*** Gnuplot script generation ***/

void print_config(int size, int sample_cnt);

void print_rotate_view(int x, int y);

void print_eps_terminal(const string& output_eps);

void print_grid_line(int x1, int y1, int x2, int y2, bool highlight);

void print_grid_rect(int left, int up, int right, int down, bool highlight);

void print_plot_command(const string& data_file, const string& color, bool replot);

void print_pause();

#endif //BSPLINE_SINGULARITIES_GALOIS_GNUPLOT_H
