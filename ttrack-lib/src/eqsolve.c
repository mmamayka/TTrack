#include <assert.h>
#include <stddef.h>
#include <math.h>
#include "eqsolve.h"
#include "comp.h"

int solve_linear(double a, double b, double eps, double* px) { // ax + b = 0
	assert(isfinite(a));
	assert(isfinite(b));
	assert(isfinite(eps));
	assert(px != NULL);

	if(about_zero(a, eps)) {
		return about_zero(b, eps) ? INF_ROOTS : 0;
	}

	*px = -b / a;
	return 1;
}

int solve_square(double a, double b, double c, double eps, 
				double* px1, double* px2) { // ax^2 + bx + c = 0
	assert(isfinite(a));
	assert(isfinite(b));
	assert(isfinite(c));
	assert(isfinite(eps));

	assert(px1 != NULL);
	assert(px2 != NULL);
	assert(px1 != px2);

	if(about_zero(a, eps)) {
		return solve_linear(b, c, eps, px1);
	}

	double D = b * b - 4 * a * c;
	double a2 = a * 2;

	if(about_zero(D, eps)) {
		*px1 = -b / a2;
		return 1;
	}
	if(D < 0) {
		return 0;
	}

	double sqrtD = sqrt(D);

	*px1 = (-b + sqrtD) / a2;
	*px2 = (-b - sqrtD) / a2;
	return 2;
}
