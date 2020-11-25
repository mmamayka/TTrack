#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include "dbg.h"
#include "strv.h"

char const* strv_errstr(strv_err_t err)
{$_
	ASSERT(err >= 0 && err < STRV_NERRORS);

	char const* TABLE[STRV_NERRORS] = {
		"ok",
		"instance pointer is null",
		"invalid state (pfirst > plast)",
		"pointer to the first character is null",
		"pointer to the last character is null"
	};
	RETURN(TABLE[err]);
$$
}

strv_t const strv_init(char const* pfirst, char const* plast)
{$_
	ASSERT(pfirst != NULL);
	ASSERT(plast != NULL);
	ASSERT(pfirst <= plast);

	strv_t strv = { pfirst, plast };
	strv_assert(&strv);

	RETURN(strv);
$$
}

strv_t const strv_make(char const* string)
{$_
	ASSERT(string != NULL);
	strv_t strv = strv_init(string, string + strlen(string));
	strv_assert(&strv);

	RETURN(strv);
$$
}

size_t const strv_len(strv_t const* strv) 
{$_
	strv_assert(strv);
	RETURN(strv->plast - strv->pfirst);
$$
}

int const strv_empty(strv_t const* strv)
{$_
	strv_assert(strv);
	RETURN(strv->pfirst == strv->plast);
$$
}

char const strv_nth(strv_t const* strv, size_t idx)
{$_
	strv_assert(strv);
	ASSERT(idx < strv_len(strv));
	RETURN(*(strv->pfirst + idx));
$$
}

char const strv_front(strv_t const* strv)
{$_
	strv_assert(strv);
	ASSERT(!strv_empty(strv));
	RETURN(*strv->pfirst);
$$
}

char const strv_back(strv_t const* strv)
{$_
	strv_assert(strv);
	ASSERT(!strv_empty(strv));
	RETURN(*(strv->plast - 1));
$$
}

char const* strv_begin(strv_t const* strv)
{$_
	strv_assert(strv);
	RETURN(strv->pfirst);
$$
}
char const* strv_end(strv_t const* strv)
{$_
	strv_assert(strv);
	RETURN(strv->plast);
$$
}

void strv_chompf(strv_t* strv)
{$_
	strv_assert(strv);

	while(strv->pfirst < strv->plast && isspace(*strv->pfirst)) {
		++strv->pfirst;
	}

	strv_assert(strv);
$$
}

void strv_chompb(strv_t* strv)
{$_
	strv_assert(strv);

	while(strv->pfirst < strv->plast && isspace(*(strv->plast - 1))) {
		--strv->plast;
	}

	strv_assert(strv);
$$
}


void strv_chomp(strv_t* strv)
{$_
	strv_assert(strv);

	strv_chompf(strv);
	strv_chompb(strv);

	strv_assert(strv);
$$
}

void strv_copy(strv_t* strv, char* buf)
{$_
	strv_assert(strv);
	ASSERT(buf != NULL);

	size_t len = strv_len(strv);
	memcpy(buf, strv->pfirst, len);
	buf[len] = '\0';
$$
}
char* strv_tostr(strv_t* strv)
{$_
	strv_assert(strv);
	size_t len = strv_len(strv);

	char* str = (char*)calloc(sizeof(char), len + 1);
	if(str == NULL) {
		RETURN(NULL);
	}

	memcpy(str, strv->pfirst, len);
	RETURN(str);
$$
}

int const strv_cmp(strv_t const* lstrv, strv_t const* rstrv) 
{$_
	strv_assert(lstrv);
	strv_assert(rstrv);

	size_t llen = strv_len(lstrv);
	size_t rlen = strv_len(rstrv);
	size_t min_len = llen < rlen ? llen : rlen;

	int cmd_res = memcmp(lstrv->pfirst, rstrv->pfirst, min_len);
	RETURN(cmd_res);
$$
}

int const strv_ccmp(strv_t const* strv, char const* cstr)
{$_
	strv_assert(strv);
	ASSERT(cstr != NULL);

	return strncmp(strv->pfirst, cstr, strv_len(strv));
$$
}

strv_t const strv_ntok(strv_t* strv, char const* delimers)
{$_
	strv_assert(strv);
	ASSERT(delimers != NULL);

	while(strv->pfirst < strv->plast && strchr(delimers, *strv->pfirst) != NULL) { 
		++strv->pfirst;
	}

	char const* rover = strv->pfirst;

	for(; strv->pfirst < strv->plast; ++strv->pfirst) {
		if(strchr(delimers, *strv->pfirst) != NULL) {
			RETURN(strv_init(rover, strv->pfirst));
		}
	}
	
	RETURN(*strv);
$$
}

size_t const strv_tok(strv_t* strv, char const* delimers, strv_t* tokens,
					  size_t max_count)
{$_
	strv_assert(strv);
	ASSERT(delimers != NULL);
	ASSERT(tokens != NULL);

	size_t count = 0;
	for(strv_t cstrv = strv_ntok(strv, delimers); 
		!strv_empty(&cstrv) && count < max_count;
		cstrv = strv_ntok(strv, delimers)) 
	{
		*tokens = cstrv;
		++tokens;
		++count;
	}

	RETURN(count);
$$
}

strv_err_t const strv_check(strv_t const* strv)
{$_
	if(strv == NULL) { RETURN(STRV_ERR_NULL); }
	if(strv->pfirst == NULL) { RETURN(STRV_ERR_PFIRST); }
	if(strv->plast == NULL) { RETURN(STRV_ERR_PLAST); }
	if(strv->pfirst > strv->plast) { RETURN(STRV_ERR_STATE); }
	RETURN(STRV_ERR_OK);
$$
}

int const strv_ok(strv_t const* strv)
{$_
	RETURN(strv_check(strv) == STRV_ERR_OK);
$$
}

void strv__dump(strv_t const* strv, char const* funcname, char const* filename,
				size_t nline)
{$_
	strv_err_t err = strv_check(strv);
	dump("strv_t [%p] dump from %s (%s %zu), reason %i (%s) {\n",
		 strv, funcname, filename, nline, err, strv_errstr(err));

	if(strv != NULL) {
		dump("\tpfirst = %p\n", strv->pfirst);
		dump("\tplast = %p\n", strv->plast);

		ptrdiff_t dif = strv->plast - strv->pfirst;
		dump("\t[diff] = %ti\n", dif);
		
		dump("\t[data] = ");

		DUMP_PUSHTFL
		DUMP_TAB_FIRST_LINE = 0;

		if(dif >= 0) {
			dump("\"");
			dump_raw(strv->pfirst, (size_t)dif);
			dump("\"\n");
		}
		else {
			dump("INVALID\n");
		}

		DUMP_POPTFL
	}
	dump("}\n");
$$
}

void strv__assert(strv_t const* strv, char const* funcname, char const* filename,
				  size_t nline)
{$_
	if(!strv_ok(strv)) {
		strv__dump(strv, funcname, filename, nline);
		ASSERT(!"invalid strv");
	}
$$
}



