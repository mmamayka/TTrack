#ifndef EQSQUARE_H
#define EQSQUARE_H

/**
 * \brief Solves square equation \f$ax^2 + bx + c = 0\f$.
 *
 *	If the equation	has only one root leaves px2 as it is; infinite number of roots
 *	leaves px1, px2 as they are.

 * \param [in] a a-coefficient.
 * \param [in] b b-coefficient.
 * \param [in] c c-coefficient.
 * \param [in] eps precision coefficient.
 * \param [out] px1 pointer to the first root of the equation.
 * \param [out] px2 pointer to the second root of the equation.
 * \return number of roots.
 */
int SolveSquare(double a, double b, double c, double eps, 
				double* px1, double* px2);


void Test_SolveSquare();

#endif
