#include "hash.h"
#include "dbg.h"

uint32_t const gnu_hash(void const* const data, size_t const size)
{$_
	ASSERT(data != NULL);

	char const* const chdata = (char const* const)data;

	uint32_t h = 5381;

	for(char const* ch = chdata; ch < chdata + size; ++ch) {
		h = h * 33 + (uint32_t)*ch;
	}

	RETURN(h);
}
