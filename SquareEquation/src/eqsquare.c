#include <assert.h>
#include <math.h>

#include "test.h"
#include "comp.h"
#include "eqlinear.h"
#include "eqsquare.h"

#ifndef INF_ROOTS
#	define INF_ROOTS
#endif

int SolveSquare(double a, double b, double c, double eps, 
				double* px1, double* px2) { // ax^2 + bx + c = 0
	assert(isfinite(a));
	assert(isfinite(b));
	assert(isfinite(c));
	assert(isfinite(eps));

	assert(px1 != NULL);
	assert(px2 != NULL);
	assert(px1 != px2);

	if(IsAboutZero(a, eps)) {
		return SolveLinear(b, c, eps, px1);
	}

	double D = b * b - 4 * a * c;
	double a2 = a * 2;

	if(IsAboutZero(D, eps)) {
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

void Test_SolveSquare() {
	double const EPS = 1e-7;
	double const POISON = 666.666;
	double x, y;

	// TEST 1
	x = y = POISON;
	TEST_IRV(SolveSquare(0.25, 0.25, 0.25, 0.5, &x, &y), INF_ROOTS);
	TEST_DV(x, POISON, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 2
	x = y = POISON;
	TEST_IRV(SolveSquare(0.25, 0.25, 10.0, 0.5, &x, &y), 0);
	TEST_DV(x, POISON, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 3
	x = y = POISON;
	TEST_IRV(SolveSquare(100.0, 1.0, 1.0, 0.5, &x, &y), 0);
	TEST_DV(x, POISON, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 4
	x = y = POISON;
	TEST_IRV(SolveSquare(4.0, 4.0, 1.1, 1.7, &x, &y), 1);
	TEST_DV(x, -0.5, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 5
	x = y = POISON;
	TEST_IRV(SolveSquare(0.0, 2.0, -10.0, 0.5, &x, &y), 1);
	TEST_DV(x, 5.0, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 6
	x = y = POISON;
	TEST_IRV(SolveSquare(2.0, 4.0, -9.0 / 8.0, EPS, &x, &y), 2);
	TEST_DV(x, (-4.0 + 5.0) / 4.0, EPS);
	TEST_DV(y, (-4.0 - 5.0) / 4.0, EPS);
}


