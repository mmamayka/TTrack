#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "eqsquare.h"

#ifdef TESTS
#	include "comp.h"
#	include "eqlinear.h"
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
		nFilledArgs = scanf("%lf %lf %lf", pa, pb, pc, eps);

		if(nFilledArgs == 3) {
			break;
		}

		printf("Damn, dude! How could you fail this?!\n");
	}
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
	int nRoots = SolveSquare(a, b, c, eps, &x1, &x2);

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
