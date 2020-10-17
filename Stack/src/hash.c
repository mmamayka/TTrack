#include <assert.h>
#include "hash.h"

uint32_t const gnu_hash(void const* const data, size_t const size) {
	assert(data != NULL);

	char const* const chdata = (char const* const)data;

	uint32_t h = 5381;

	for(char const* ch = chdata; ch < chdata + size; ++ch) {
		h = h * 33 + (uint32_t)*ch;
	}

	return h;
}
