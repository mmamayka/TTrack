#ifndef COMP_H
#define COMP_H

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

#include <stddef.h>

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

size_t const next_2power(size_t v);

#endif /* COMP_H */
