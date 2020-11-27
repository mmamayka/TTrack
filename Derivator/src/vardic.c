#include <math.h>
#include <ttrack/dbg.h>
#include "vardic.h"

ENUMEX_SOURCE(vardic_err,
	"ok",
	"instance pointer is null",
	"invalid name",
	"invalid value"
);

vardic_err_t const vardic_check(vardic_t* vardic)
{$_
	if(vardic == NULL) { RETURN(VARDIC_ERR_NULL); }
	RETURN(VARDIC_ERR_OK);
$$
}

int const vardic_ok(vardic_t* vardic)
{$_
	RETURN(vardic_check(vardic) == VARDIC_ERR_OK);
$$
}

void vardic__dump(vardic_t* vardic, char const* funcname, char const* filename,
				  size_t nline)
{$_
	vardic_err_t err = vardic_check(vardic);
	dump("vardic_t [%p] dump from %s (%s %zu), reason %i (%s) = {\n",
		 vardic, funcname, filename, nline, err, vardic_err_str(err));

	if(vardic != NULL) {
		dump("\tdata[VARDIC_SIZE = %zu] = {\n", VARDIC_SIZE);

		for(size_t i = 0; i < VARDIC_SIZE; ++i) {
			dump("\t\t[%zu] = %lf\n", i, vardic->data[i]);
		}
	}

	dump("}\n");
$$
}

void vardic__assert(vardic_t* vardic, char const* funcname, char const* filename,
					size_t nline)
{$_
	if(!vardic_ok(vardic)) {
		vardic__dump(vardic, funcname, filename, nline);
		ASSERT_(!"invalid vardic", funcname, filename, nline);
	}
$$
}

vardic_err_t const vardic_init(vardic_t* vardic)
{$_
	ASSERT(vardic != NULL);

	for(size_t i = 0; i < VARDIC_SIZE; ++i) {
		vardic->data[i] = NAN;
	}

	RETURN(VARDIC_ERR_OK);
$$
}

vardic_err_t const vardic_set(vardic_t* vardic, char name, double value)
{$_
	vardic_assert(vardic);
	vardic_name_assert(vardic, name);
	ASSERT(!isnan(value));

	vardic->data[name - 'a'] = value;
	RETURN(VARDIC_ERR_OK);
$$
}

vardic_err_t const vardic_get(vardic_t* vardic, char name, double* pvalue)
{$_
	vardic_assert(vardic);
	vardic_name_assert(vardic, name);
	ASSERT(pvalue != NULL);

	*pvalue = vardic->data[name - 'a'];
	RETURN(VARDIC_ERR_OK);
$$
}

int const vardic_name_ok(vardic_t* vardic, char name)
{$_
	vardic_assert(vardic);
	(void)vardic;

	RETURN(name - 'a' >= 0 && (size_t)(name - 'a') < VARDIC_SIZE);
$$
}

void vardic__name_assert(vardic_t* vardic, char name, char const* funcname, 
						  char const* filename, size_t nline)
{$_
	vardic_assert(vardic);

	if(!vardic_name_ok(vardic, name)) {
		ASSERT_(!"invalid name in variables dictionary", funcname, filename, nline);
	}
$$
}


