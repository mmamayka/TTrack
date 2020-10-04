#include <assert.h>
#include <math.h>

#include "eqlinear.h"
#include "comp.h"
#include "test.h"

int SolveLinear(double a, double b, double eps, double* px) { // ax + b = 0
	assert(isfinite(a));
	assert(isfinite(b));
	assert(isfinite(eps));
	assert(px != NULL);

	if(IsAboutZero(a, eps)) {
		return IsAboutZero(b, eps) ? INF_ROOTS : 0;
	}

	*px = -b / a;
	return 1;
}

void Test_SolveLinear() {
	double const EPS = 1e-7;
	double const POISON = 666.666;
	double x;

	// TEST 1
	x = POISON;
	TEST_IRV(SolveLinear(0.25, 5.0, 0.5, &x), 0);
	TEST_DV(x, POISON, EPS);

	// TEST 2
	x = POISON;
	TEST_IRV(SolveLinear(0.25, 0.25, 0.5, &x), INF_ROOTS);
	TEST_DV(x, POISON, EPS);

	// TEST 3
	x = POISON;
	TEST_IRV(SolveLinear(5.0, -10.0, 0.5, &x), 1);
	TEST_DV(x, 2.0, EPS);
}
