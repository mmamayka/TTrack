#ifndef EQLINEAR_H
#define EQLINEAR_H

#define INF_ROOTS -1

/**
 *	\brief Solves linear equation \f$ax + b = 0\f$.
 *	
 *	If the equation has infinite number of roots leaves px as it is.
 *
 *	\param [in] a a-coefficient.
 *	\param [in] b b-coefficient.
 *	\param [in] eps precision coefficient.
 *	\param [out] px pointer to root.
 *	\return number of roots (INF_ROOTS if the equation has infinite number of roots).
 */
int SolveLinear(double a, double b, double eps, double* px);

void Test_SolveLinear();

#endif
