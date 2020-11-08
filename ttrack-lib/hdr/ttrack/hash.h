#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */


uint32_t const gnu_hash(void const* const data, size_t const size);


#endif /* HASH_H */
