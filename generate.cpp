#include <iostream>
#include <vector>
#include "domain.h"
#include "bspline-non-rect.h"

using namespace std;


Cube get_outmost_box(Coord size, MeshShape shape) {
	if (shape == QUADRATIC) {
		return Cube(0, size, 0, size);
	} else if (shape == RECTANGULAR) {
		return Cube(0, (Coord) (1.5 * size), 0, size);
	}
}

Cube get_inner_box(Coord middle, Coord edge_offset) {
	return Cube(middle - edge_offset, middle + edge_offset,
				middle - edge_offset, middle + edge_offset);
}

Cube get_inner_box(const Cube &outer_box, Coord edge_offset) {
	return Cube(outer_box.get_bound(0) + edge_offset, outer_box.get_bound(1) - edge_offset,
				outer_box.get_bound(2) + edge_offset, outer_box.get_bound(3) - edge_offset);
}

int main(int argc, char** argv) {

	enum OutputFormat {
		DRAW_NEIGHBORS,
		DRAW_PLAIN,
		DRAW_SUPPORTS,
		GALOIS,
		GNUPLOT,
		KNOTS
	} output_format = GALOIS;

	if (argc >= 2) {
		bool any_opt = true;
		string opt(argv[1]);
		if (opt == "-n" || opt == "--draw-neighbors")
			output_format = DRAW_NEIGHBORS;
		else if (opt == "-d" || opt == "--draw-plain")
			output_format = DRAW_PLAIN;
		else if (opt == "-s" || opt == "--draw-supports")
			output_format = DRAW_SUPPORTS;
		else if (opt == "-g" || opt == "--galois")
			output_format = GALOIS;
		else if (opt == "-p" || opt == "--gnuplot")
			output_format = GNUPLOT;
		else if (opt == "-k" || opt == "--knots")
			output_format = KNOTS;
		else
			any_opt = false;
		if (any_opt) {
			argc--;
			argv++;
		}
	}

	MeshType mesh_type = EDGED_4;

	if (argc >= 2) {
		bool any_mesh = true;
		string mesh(argv[1]);
		if (mesh == "--unedged" || mesh == "-u")
			mesh_type = UNEDGED;
		else if (mesh == "--edged-4" || mesh == "-4")
			mesh_type = EDGED_4;
		else if (mesh == "--edged-8" || mesh == "-8")
			mesh_type = EDGED_8;
		else
			any_mesh = false;
		if (any_mesh) {
			argc--;
			argv++;
		}
	}

	MeshShape mesh_shape = QUADRATIC;

	if (argc >= 2) {
		bool any_shape = true;
		string mesh(argv[1]);
		if (mesh == "--quadratid" || mesh == "-q")
			mesh_shape = QUADRATIC;
		else if (mesh == "--rectangular" || mesh == "-r")
			mesh_shape = RECTANGULAR;
		else
			any_shape = false;
		if (any_shape) {
			argc--;
			argv++;
		}
	}

	int depth;
	if (argc >= 2) {
		depth = atoi(argv[1]);
		argc--;
		argv++;
	} else {
		depth = 3;
	}

	int order;
	if (argc >= 2) {
		order = atoi(argv[1]);
	} else {
		order = 2;
	}


	Coord size = (output_format == GALOIS ? 4L : 2L) << depth;  // so that the smallest elements are of size 1x1
	//cout << "size: " << size << endl;
	Cube outmost_box(get_outmost_box(size, mesh_shape));
	//outmost_box.print_bounds();
	//cout << endl;
	Domain domain(outmost_box);

	Coord middle = size / 2;
	Coord edge_offset = size / 4;
	Cube outer_box = outmost_box;

	// Build a regular 4x4 grid.
	if (mesh_shape == QUADRATIC) {
		domain.split_all_elements_into_4_2D();  // 1 -> 4 elements (2x2)
		domain.split_all_elements_into_4_2D();  // 4 -> 16 elements (4x4)
		if (mesh_type == EDGED_8 && depth > 1)
			domain.split_eight_side_elements_within_box_2D(outmost_box);

		// Generate the adapted grid.
		for (int i = 1; i < depth; i++) {
			Cube inner_box(get_inner_box(middle, edge_offset));

			if (mesh_type == EDGED_4 || mesh_type == EDGED_8) {
				bool edged_8 = mesh_type == EDGED_8;
				domain.add_edge_2D(X_DIM, outer_box, inner_box.up(), 4, edged_8);  // horizontal
				domain.add_edge_2D(X_DIM, outer_box, inner_box.down(), 4, edged_8);
				domain.add_edge_2D(Y_DIM, outer_box, inner_box.left(), 4, edged_8);  // vertical
				domain.add_edge_2D(Y_DIM, outer_box, inner_box.right(), 4, edged_8);
				domain.add_corner_vertices_2D(inner_box);
			}

			// Internal 4 elements -> 16 elements
			domain.split_elements_within_box_into_4_2D(inner_box);
			if (mesh_type == EDGED_8 && i < depth - 1)
				domain.split_eight_side_elements_within_box_2D(inner_box);

			edge_offset /= 2;
			outer_box = inner_box;
		}
	} else if (mesh_shape == RECTANGULAR) {
		//build rectangular mesh, 4x6 grid (edge)
		domain.split_all_elements_into_6_2D();  // 1 -> 6 elements (2x3)
		domain.split_all_elements_into_4_2D();  // 4 -> 24 elements (4x6)

		// Generate the adapted grid.
		for (int i = 1; i < depth; i++) {
			Cube inner_box(get_inner_box(outer_box, edge_offset));

			domain.add_edge_2D(X_DIM, outer_box, inner_box.up(), 6, false);  // horizontal
			domain.add_edge_2D(X_DIM, outer_box, inner_box.down(), 6, false);
			domain.add_edge_2D(Y_DIM, outer_box, inner_box.left(), 4, false);  // vertical
			domain.add_edge_2D(Y_DIM, outer_box, inner_box.right(), 4, false);
			domain.add_corner_vertices_2D(inner_box);

			//Split internal elements
			domain.split_elements_within_box_into_4_2D(inner_box);

			edge_offset /= 2;
			outer_box = inner_box;
		}
	}

	domain.allocate_elements_count_by_level_vector(depth);
	domain.enumerate_all_elements();
	domain.tweak_bounds();
	domain.compute_all_neighbors(size);
	domain.untweak_bounds();

	if (output_format == DRAW_NEIGHBORS) {
		domain.tweak_bounds();  // again, just for printing
		domain.print_all_elements();
		domain.print_all_neighbors();

	} else if (output_format == DRAW_SUPPORTS) {
		domain.print_all_elements();
		domain.compute_bsplines_supports(mesh_type, order);
		domain.print_support_for_each_bspline();

	} else if (output_format == GALOIS) {
		domain.compute_bsplines_supports(mesh_type, order);

		edge_offset = size / 4;
		outer_box = outmost_box;

		Node * outer_node = domain.add_tree_node(outer_box, NULL);
		Node * side_node;

		// Generate elimination tree.
		for (int i = 1; i < depth; i++) {
			//cout << "looping" << endl;
			Cube inner_box(get_inner_box(outer_box, edge_offset));
			Cube side_box, main_box;

			outer_box.split(X_DIM, inner_box.left(), &side_box, &main_box);
			side_node = domain.add_tree_node(side_box, outer_node);
			domain.tree_process_cut_off_box(Y_DIM, side_node, false);
			outer_node = domain.add_tree_node(main_box, outer_node);
			outer_box = main_box;
			domain.tree_process_box_2D(side_box);

			outer_box.split(X_DIM, inner_box.right(), &main_box, &side_box);
			side_node = domain.add_tree_node(side_box, outer_node);
			outer_node = domain.add_tree_node(main_box, outer_node);
			domain.tree_process_cut_off_box(Y_DIM, side_node, false);
			outer_box = main_box;
			domain.tree_process_box_2D(side_box);

			outer_box.split(Y_DIM, inner_box.up(), &side_box, &main_box);
			side_node = domain.add_tree_node(side_box, outer_node);
			outer_node = domain.add_tree_node(main_box, outer_node);
			domain.tree_process_cut_off_box(X_DIM, side_node, false);
			outer_box = main_box;
			domain.tree_process_box_2D(side_box);

			outer_box.split(Y_DIM, inner_box.down(), &main_box, &side_box);
			side_node = domain.add_tree_node(side_box, outer_node);
			outer_node = domain.add_tree_node(main_box, outer_node);
			domain.tree_process_cut_off_box(X_DIM, side_node, false);
			outer_box = main_box;
			domain.tree_process_box_2D(side_box);

			edge_offset /= 2;
		}
		// The innermost 16 elements are processed at the very end.
		domain.tree_process_cut_off_box(X_DIM, outer_node, true);

		domain.print_galois_output();

	} else if (output_format == DRAW_PLAIN || output_format == GNUPLOT) {
		domain.print_all_elements();

	} else if (output_format == KNOTS) {
		domain.print_all_elements();
		domain.compute_bsplines_supports(mesh_type, order);
		domain.print_knots_for_each_bspline();
	}

	return 0;
}
