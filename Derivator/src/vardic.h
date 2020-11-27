#ifndef VARDIC_H
#define VARDIC_H

#include "enumex.h"

#define VARDIC_SIZE (size_t)('z' - 'a' + 1)

ENUMEX_HEADER(vardic_err,
	VARDIC_ERR_OK,
	VARDIC_ERR_NULL,
	VARDIC_ERR_NAME,
	VARDIC_ERR_VALUE
);

#define vardic_err_assert(err) \
	vardic_err__assert(err, __func__, __FILE__, __LINE__)

typedef struct {
	double data[VARDIC_SIZE];
} vardic_t;

vardic_err_t const vardic_check(vardic_t* vardic);
int const vardic_ok(vardic_t* vardic);

void vardic__dump(vardic_t* vardic, char const* funcname, char const* filename,
				  size_t nline);

#define vardic_dump(vardic) \
	vardic__dump(vardic, __func__, __FILE__, __LINE__)

void vardic__assert(vardic_t* vardic, char const* funcname, char const* filename,
					size_t nline);

#define vardic_assert(vardic) \
	vardic__assert(vardic, __func__, __FILE__, __LINE__)

vardic_err_t const vardic_init(vardic_t* vardic);
vardic_err_t const vardic_set(vardic_t* vardic, char name, double value);
vardic_err_t const vardic_get(vardic_t* vardic, char name, double* pvalue);

int const vardic_name_ok(vardic_t* vardic, char name);
void vardic__name_assert(vardic_t* vardic, char name, char const* funcname, 
						  char const* filename, size_t nline);

#define vardic_name_assert(vardic, index) \
	vardic__name_assert(vardic, index, __func__, __FILE__, __LINE__)

#endif
