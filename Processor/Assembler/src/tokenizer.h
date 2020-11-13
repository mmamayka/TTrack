#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>

typedef enum {
	TOKENIZER_ERR_OK = 0,
	TOKENIZER_ERR_MEM,
	TOKENIZER_ERR_STDIO,
	TOKENIZER_ERR_STATE,
	TOKENIZER_ERR_EOF,
	TOKENIZER_ERR_OVERFLOW,

	TOKENIZER_NERRORS
} tokenizer_err_t;

char const* tokenizer_errstr(tokenizer_err_t errc);

tokenizer_err_t const tokenizer_init(FILE* file);
void tokenizer_free();

tokenizer_err_t const tokenizer_nextline(size_t max_args, char* args[], size_t* num);
size_t const tokenizer_nline();

void tokenizer_reset();

tokenizer_err_t const tokenizer_check();
void tokenizer__dump(char const* funcname, char const* filename, size_t nline);
void tokenizer__assert(char const* funcname, char const* filename, size_t nline);


#define tokenizer_dump() \
	tokenizer__dump(__func__, __FILE__, __LINE__)

#define tokenizer_assert() \
	tokenizer__assert(__func__, __FILE__, __LINE__)

#endif
