#include <cstdio>
#include <vector>
using namespace std;

double bspline_dyn(const vector<double>& nodes, double point) { 

	int order = nodes.size() - 2;

	// 0..order o's, 0..order b's
	double* values = new double[(order+1)*(order+1)];
	#define val(o,b) values[(o) * (order+1) + (b)]

	for (int o = 0; o <= order; o++) {
		for (int b = 0; b <= order-o; b++) {
			if (o == 0) {
				bool point_covered = nodes[b] <= point && point < nodes[b+1];
				val(0, b) = point_covered ? 1.0 : 0.0;
			} else {
				double left_num  = point - nodes[b];
				double left_den  = nodes[b+o] - nodes[b];
				double right_num = nodes[b+o+1] - point;
				double right_den = nodes[b+o+1] - nodes[b+1];
			
				double left = 0.0, right = 0.0;
				if (left_den != 0.0)
					left = left_num  / left_den * val(o-1, b); 
				if (right_den != 0.0)
					right = right_num / right_den * val(o-1, b+1);

				//printf("left = %lf, left_val = %lf, left_num = %lf, left_den = %lf\nright = %lf, right_val = %lf, right_num = %lf, right_den = %lf\n",
					//left, val(o-1,b), left_num, left_den, right, val(o-1,b+1), right_num, right_den);
				val(o, b) = left + right;
			}
			//printf("values(order=%i, no=%i) = %lf\n", o, b, val(o, b));
		}
	}

	double result = val(order, 0);

	//printf("bspline(no = %i, order = %i, point = %lf) = %lf\n", no, order, point, result);

	delete values;
	return result;
}

int main() {
	vector<double> nodes = { 0, 1, 1, 2 };
	for (double x = 0.0; x <= 2.0; x += 0.25) {
		printf("%lf %lf\n", x, bspline_dyn(nodes, x));
	}
}

