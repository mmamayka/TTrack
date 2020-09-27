#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPS 1e-7
#define INF_ROOTS -1

/**
 * \brief Reads a, b, c coefficients of square equation from console.
 *
 * \param [out] pa pointer to a-coefficient of square squation.
 * \param [out] pb pointer to b-coefficient of square squation.
 * \param [out] pc pointer to c-coefficient of square squation.
 */
void ReadCoeff(double* pa, double* pb, double* pc) {
	assert(pa != NULL);
	assert(pb != NULL);
	assert(pc != NULL);

	assert(pa != pb);
	assert(pb != pc);
	assert(pc != pa);

	int nFilledArgs = 0;

	for(;;) {
		printf("Enter a, b, c: ");
		nFilledArgs = scanf("%lf %lf %lf", pa, pb, pc);

		if(nFilledArgs == 3) {
			break;
		}

		printf("Damn, dude! How could you fail this?!\n");
	}
}

/**
 *	\brief Tests value on zero similarity.
 *
 *	Compares if |x| less than EPS value.
 *
 *	\param [in] x the value.
 *	\return boolean result of comparison.
 */
int IsAboutZero(double x) {
	assert(isfinite(x));

	return fabs(x) < EPS;
}

/**
 *	\brief Solves linear equation \f$ax + b = 0\f$.
 *	
 *	If the equation has infinite number of roots leaves px as it is.
 *
 *	\param [in] a a-coefficient.
 *	\param [in] b b-coefficient.
 *	\param [out] px pointer to root.
 *	\return number of roots (INF_ROOTS if the equation has infinite number of roots).
 */
int SolveLinear(double a, double b, double* px) { // ax + b = 0
	assert(isfinite(a));
	assert(isfinite(b));
	assert(px != NULL);

	if(IsAboutZero(a)) {
		return IsAboutZero(b) ? INF_ROOTS : 0;
	}

	*px = -b / a;
	return 1;
}

/**
 * \brief Solves square equation \f$ax^2 + bx + c = 0\f$.
 *
 *	If the equation	has only one root leaves px2 as it is; infinite number of roots
 *	leaves px1, px2 as they are.

 * \param [in] a a-coefficient.
 * \param [in] b b-coefficient.
 * \param [in] c c-coefficient.
 * \param [out] px1 pointer to the first root of the equation.
 * \param [out] px2 pointer to the second root of the equation.
 * \return number of roots.
 */
int SolveSquare(double a, double b, double c, double* px1, double* px2) { // ax^2 + bx + c = 0
	assert(isfinite(a));
	assert(isfinite(b));
	assert(isfinite(c));

	assert(px1 != NULL);
	assert(px2 != NULL);
	assert(px1 != px2);

	if(IsAboutZero(a)) {
		return SolveLinear(b, c, px1);
	}

	double D = b * b - 4 * a * c;
	double a2 = a * 2;

	if(IsAboutZero(D)) {
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

int main() {
	printf("# Square Equation Solver\n");
	printf("# Mamay Ilya, 2020\n\n");

	double a = NAN, b = NAN, c = NAN;
	ReadCoeff(&a, &b, &c);

	double x1 = NAN, x2 = NAN;
	int nRoots = SolveSquare(a, b, c, &x1, &x2);

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
}
