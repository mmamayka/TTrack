#include <assert.h>
#include <math.h>
#include "comp.h"
#include "test.h"

int IsAboutZero(double x, double eps) {
	assert(isfinite(x));
	assert(isfinite(eps));

	return fabs(x) <= eps;
}
void Test_IsAboutZero() {
	TEST_IRV(IsAboutZero(1.0, 0.5),   0);
	TEST_IRV(IsAboutZero(-1.0, 0.5),  0);
	TEST_IRV(IsAboutZero(0.25, 0.5),  1);
	TEST_IRV(IsAboutZero(-0.25, 0.5), 1);
}

int IsAbout(double x, double y, double eps) {
	assert(isfinite(x));
	assert(isfinite(y));
	assert(isfinite(eps));

	return IsAboutZero(x - y, eps);
}
void Test_IsAbout() {
	TEST_IRV(IsAbout(2.5, 2.0, 0.25), 0);
	TEST_IRV(IsAbout(2.0, 2.5, 0.25), 0);
	TEST_IRV(IsAbout(2.25, 2.0, 0.5), 1);
	TEST_IRV(IsAbout(2.0, 2.25, 0.5), 1);
}
