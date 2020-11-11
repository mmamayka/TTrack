#include <math.h>
#include <stdint.h>
#include "comp.h"
#include "dbg.h"

int about_zero(double x, double eps) 
{$_
	ASSERT(isfinite(x));
	ASSERT(isfinite(eps));

	RETURN(fabs(x) <= eps);
}

int about(double x, double y, double eps) 
{$_
	ASSERT(isfinite(x));
	ASSERT(isfinite(y));
	ASSERT(isfinite(eps));

	RETURN(about_zero(x - y, eps));
}


size_t const next_2power(size_t v)
{
	--v;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v |= v >> 32;
	++v;
	return v;
}
