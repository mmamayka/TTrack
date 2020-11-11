#include <stddef.h>
#include <math.h>
#include "eqsolve.h"
#include "comp.h"
#include "dbg.h"

int solve_linear(double a, double b, double eps, double* px) 
{$_
	ASSERT(isfinite(a));
	ASSERT(isfinite(b));
	ASSERT(isfinite(eps));
	ASSERT(px != NULL);

	if(about_zero(a, eps)) {
		RETURN(about_zero(b, eps) ? INF_ROOTS : 0);
	}

	*px = -b / a;
	RETURN(1);
}

int solve_square(double a, double b, double c, double eps, 
				double* px1, double* px2) 
{$_
	ASSERT(isfinite(a));
	ASSERT(isfinite(b));
	ASSERT(isfinite(c));
	ASSERT(isfinite(eps));

	ASSERT(px1 != NULL);
	ASSERT(px2 != NULL);
	ASSERT(px1 != px2);

	if(about_zero(a, eps)) {
		RETURN(solve_linear(b, c, eps, px1));
	}

	double D = b * b - 4 * a * c;
	double a2 = a * 2;

	if(about_zero(D, eps)) {
		*px1 = -b / a2;
		RETURN(1);
	}
	if(D < 0) {
		RETURN(0);
	}

	double sqrtD = sqrt(D);

	*px1 = (-b + sqrtD) / a2;
	*px2 = (-b - sqrtD) / a2;
	RETURN(2);
}
