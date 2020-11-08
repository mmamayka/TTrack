#ifndef EQSOLVE_H
#define EQSOLVE_H

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
int solve_linear(double a, double b, double eps, double* px);

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
int solve_square(double a, double b, double c, double eps, 
				double* px1, double* px2);



#endif
