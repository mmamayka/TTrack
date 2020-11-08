#ifndef COMP_H
#define COMP_H

/**
 *	\brief Tests value on zero similarity.
 *
 *	Compares if |x| less than eps value.
 *
 *	\param [in] x the value.
 *	\param [in] eps precision coefficient.
 *	\return boolean result of comparison.
 */
int about_zero(double x, double eps);

/**
 *	\brief Tests two values on similarity.
 *
 *	Compares if |x - y| less than eps value.
 *
 *	\param [in] x the first value.
 *	\param [in] y the second value.
 *	\param [in] eps precision coefficient.
 *	\return boolean result of comparison.
 */
int about(double x, double y, double eps);

#endif /* COMP_H */
