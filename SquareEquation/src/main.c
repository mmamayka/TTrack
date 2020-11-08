#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ttrack/eqsolve.h>

#ifdef TESTS
#	include <ttrack/test.h>
#endif

/**
 * \brief Reads a, b, c coefficients of square equation from console
 *
 * \param [out] pa pointer to a-coefficient of square equation
 * \param [out] pb pointer to b-coefficient of square equation
 * \param [out] pc pointer to c-coefficient of square equation
 */
void ReadCoeff(double* pa, double* pb, double* pc, double* eps);

void ReadCoeff(double* pa, double* pb, double* pc, double* eps) {
	assert (pa  != NULL);
	assert (pb  != NULL);
	assert (pc  != NULL);
	assert (eps != NULL);

	assert(pa != pb);
	assert(pb != pc);
	assert(pc != pa);
	assert(pa != eps);
	assert(pb != eps);
	assert(pc != eps);

	int nFilledArgs = 0;

	for(;;) {
		printf("Enter a, b, c, eps: ");
		nFilledArgs = scanf("%lf%lf%lf%lf", pa, pb, pc, eps);

		if(nFilledArgs == 4) {
			break;
		}

		// scanf("%*[^\n]");
		fflush(stdin);

		printf("Damn, dude! How could you fail this?!\n");
	}
}
void Test_IsAbout() {
	TEST_IRV(about(2.5, 2.0, 0.25), 0);
	TEST_IRV(about(2.0, 2.5, 0.25), 0);
	TEST_IRV(about(2.25, 2.0, 0.5), 1);
	TEST_IRV(about(2.0, 2.25, 0.5), 1);
}
void Test_IsAboutZero() {
	TEST_IRV(about_zero(1.0, 0.5),   0);
	TEST_IRV(about_zero(-1.0, 0.5),  0);
	TEST_IRV(about_zero(0.25, 0.5),  1);
	TEST_IRV(about_zero(-0.25, 0.5), 1);
}

void Test_SolveLinear() {
	double const EPS = 1e-7;
	double const POISON = 666.666;
	double x;

	// TEST 1
	x = POISON;
	TEST_IRV(solve_linear(0.25, 5.0, 0.5, &x), 0);
	TEST_DV(x, POISON, EPS);

	// TEST 2
	x = POISON;
	TEST_IRV(solve_linear(0.25, 0.25, 0.5, &x), INF_ROOTS);
	TEST_DV(x, POISON, EPS);

	// TEST 3
	x = POISON;
	TEST_IRV(solve_linear(5.0, -10.0, 0.5, &x), 1);
	TEST_DV(x, 2.0, EPS);
}

void Test_SolveSquare() {
	double const EPS = 1e-7;
	double const POISON = 666.666;
	double x, y;

	// TEST 1
	x = y = POISON;
	TEST_IRV(solve_square(0.25, 0.25, 0.25, 0.5, &x, &y), INF_ROOTS);
	TEST_DV(x, POISON, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 2
	x = y = POISON;
	TEST_IRV(solve_square(0.25, 0.25, 10.0, 0.5, &x, &y), 0);
	TEST_DV(x, POISON, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 3
	x = y = POISON;
	TEST_IRV(solve_square(100.0, 1.0, 1.0, 0.5, &x, &y), 0);
	TEST_DV(x, POISON, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 4
	x = y = POISON;
	TEST_IRV(solve_square(4.0, 4.0, 1.1, 1.7, &x, &y), 1);
	TEST_DV(x, -0.5, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 5
	x = y = POISON;
	TEST_IRV(solve_square(0.0, 2.0, -10.0, 0.5, &x, &y), 1);
	TEST_DV(x, 5.0, EPS);
	TEST_DV(y, POISON, EPS);

	// TEST 6
	x = y = POISON;
	TEST_IRV(solve_square(2.0, 4.0, -9.0 / 8.0, EPS, &x, &y), 2);
	TEST_DV(x, (-4.0 + 5.0) / 4.0, EPS);
	TEST_DV(y, (-4.0 - 5.0) / 4.0, EPS);
}

int main() {
#ifdef TESTS
	Test_IsAboutZero();
	Test_IsAbout();
	Test_SolveLinear();
	Test_SolveSquare();
#endif

	printf("# Square Equation Solver\n");
	printf("# Mamay Ilya, 2020\n\n");

	double a = NAN, b = NAN, c = NAN, eps = NAN;
	ReadCoeff(&a, &b, &c, &eps);

	double x1 = NAN, x2 = NAN;
	int nRoots = solve_square(a, b, c, eps, &x1, &x2);

	switch(nRoots) {
	case 0:
		printf("No roots\n");
		break;

	case 1:
		printf("x = %lf\n", x1);
		break;

	case 2:
		printf("x1 = %lf, x2 = %lf\n", x1, x2);
		break;

	case INF_ROOTS:
		printf("Infinite number of roots\n");
		break;

	default:
		printf("ERRRRROR: SMTH AWFUL HAPPEN\n");
		return EXIT_FAILURE;
		break;
	}

	return EXIT_SUCCESS;
};
