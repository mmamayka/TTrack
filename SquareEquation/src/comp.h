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
int IsAboutZero(double x, double eps);

void Test_IsAboutZero();

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
int IsAbout(double x, double y, double eps);

void Test_IsAbout();

#endif /* COMP_H */
