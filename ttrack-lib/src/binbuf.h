#ifndef BINBUF_H
#define BINBUF_H

#include <stdint.h>
#include <stdio.h>
#include "hash.h"

typedef enum {
	BINBUF_ERR_OK = 0,
	BINBUF_ERR_MEM,
	BINBUF_ERR_RANGE,
	BINBUF_ERR_DATA,
	BINBUF_ERR_STDIO,
	BINBUF_ERR_ERRRANGE,
	BINBUF_ERR_HASH,
	BINBUF_NERRORS
} binbuf_err_t;

char const* binbuf_errstr(binbuf_err_t errc);

binbuf_err_t const binbuf_init(size_t capacity);
binbuf_err_t const binbuf_initf(FILE* stream);

binbuf_err_t const binbuf_inithf(FILE* stream);

void binbuf_free();

binbuf_err_t binbuf_check();
void binbuf__dump(char const* funcname, char const* filename, size_t nline);
void binbuf__assert(char const* funcname, char const* filename, size_t nline);

#define binbuf_dump() binbuf__dump(__func__, __FILE__, __LINE__)
#define binbuf_assert() binbuf__assert(__func__, __FILE__, __LINE__)

binbuf_err_t const binbuf_write(void const* data, size_t size);
binbuf_err_t const binbuf_read(void* data, size_t size);
size_t const binbuf_pos();

#define binbuf_write_value(type, value) \
	binbuf_write(&value, sizeof(type))

#define binbuf_read_value(type, value) \
	binbuf_read(&value, sizeof(type))

binbuf_err_t const binbuf_flush(FILE* stream);
binbuf_err_t const binbuf_flushh(FILE* stream);

void binbuf_reset();
binbuf_err_t const binbuf_seek(size_t pos);

binbuf_err_t const binbuf_error();
void binbuf_clearerr();

int const binbuf_ok();


#endif /* BINBUF_H */
