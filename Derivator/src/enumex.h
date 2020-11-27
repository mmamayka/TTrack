#ifndef ENUMEX_H
#define ENUMEX_H

#include <ttrack/dbg.h>

#define ENUMEX_HEADER(enum_stem, ...) 											\
	typedef enum { __VA_ARGS__, enum_stem ## __COUNT } enum_stem ## _t;			\
	int const enum_stem ## _ok(enum_stem ## _t x);								\
	void enum_stem ## __assert(enum_stem ## _t x, char const* funcname, 		\
									char const* filename, size_t nline);		\
	char const* enum_stem ## _str(enum_stem ## _t x);

#define ENUMEX_SOURCE(enum_stem, ...) 											\
	int const enum_stem ## _ok (enum_stem ## _t x)								\
	{$_																			\
		RETURN(x >= 0 && x < enum_stem ## __COUNT);								\
	$$																			\
	}																			\
																				\
	void enum_stem ## __assert(enum_stem ## _t x, char const* funcname, 		\
							   char const* filename, size_t nline)				\
	{$_																			\
		if(!enum_stem ## _ok(x)) {												\
			ASSERT_(!"invalid enum value", funcname, filename, nline);			\
		}																		\
	$$																			\
	}																			\
																				\
	char const* enum_stem ## _str(enum_stem ## _t x) 							\
	{$_																			\
		enum_stem ## __assert (x, __func__, __FILE__, __LINE__);				\
		char const* TABLE[enum_stem ## __COUNT] = { __VA_ARGS__ };				\
		RETURN(TABLE[x]);														\
	$$																			\
	}


#endif
