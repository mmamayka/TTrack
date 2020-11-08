#include <assert.h>
#include <math.h>
#include "comp.h"
#include "test.h"

int about_zero(double x, double eps) {
	assert(isfinite(x));
	assert(isfinite(eps));

	return fabs(x) <= eps;
}

int about(double x, double y, double eps) {
	assert(isfinite(x));
	assert(isfinite(y));
	assert(isfinite(eps));

	return about_zero(x - y, eps);
}

