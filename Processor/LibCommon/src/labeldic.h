#ifndef LABELDIC_H
#define LABELDIC_H

#include <stddef.h>

#define LABELDIC_MAX_LABELS 16
#define LABELDIC_POISON SIZE_MAX

typedef enum {
	LABELDIC_ERR_OK,
	LABELDIC_ERR_OVERFLOW,
	LABELDIC_ERR_STATE,
	LABELDIC_ERR_MEM,
	LABELDIC_ERR_ERRANGE,
	LABELDIC_NERRORS
} labeldic_err_t;

char const* labeldic_errstr(labeldic_err_t errc);

labeldic_err_t const labeldic_getaddr(char const* name, size_t* addr);
labeldic_err_t const labeldic_setaddr(char const* name, size_t addr);

char const* labeldic_addrname(size_t addr);

size_t const labeldic_size();

labeldic_err_t const labeldic_error();
void const labeldic_clear_error();

labeldic_err_t const labeldic_check();
void labeldic__dump(char const* funcname, char const* filename, size_t nline);
void labeldic__assert(char const* funcname, char const* filename, size_t nline);

void labeldic_free();

#define labeldic_dump() \
	labeldic__dump(__func__, __FILE__, __LINE__)

#define labeldic_assert() \
	labeldic__assert(__func__, __FILE__, __LINE__)

#endif
