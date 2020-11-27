#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <ttrack/strv.h>
#include "enumex.h"

ENUMEX_HEADER(token_type,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_VARNAME,
	TOKEN_NUMBER,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_POW,
	TOKEN_EOF,
	TOKEN_UNKNOWN
);

#define token_type_assert(token) \
	token_type__assert(token, __func__, __FILE__, __LINE__)

ENUMEX_HEADER(token_err,
	TOKEN_ERR_OK,
	TOKEN_ERR_NULL,
	TOKEN_ERR_INVTYPE,
	TOKEN_ERR_INVVAL,
	TOKEN_ERR_INVNAME
);

#define token_err_assert(err) \
	token_err__assert(token, __func__, __FILE__, __LINE__)

typedef struct {
	token_type_t type;

	union {
		double value;
		char name;
	};
} token_t;

token_err_t const token_check(token_t* token);
int const token_ok(token_t* token);

void token__dump(token_t* token, char const* funcname, char const* filename, 
				 size_t nline);

void token__assert(token_t* token, char const* funcname, char const* filename,
				   size_t nline);

#define token_dump(token) \
	token__dump(token, __func__, __FILE__, __LINE__)

#define token_assert(token) \
	token__assert(token, __func__, __FILE__, __LINE__)

token_t const token_triv(token_type_t type);
token_t const token_varname(char name);
token_t const token_number(double value);

ENUMEX_HEADER(tokenizer_err,
	TOKENIZER_ERR_OK,
	TOKENIZER_ERR_NULL,
	TOKENIZER_ERR_NF,
	TOKENIZER_ERR_MEM,
	TOKENIZER_ERR_STATE,
	TOKENIZER_ERR_STDIO
);

#define tokenizer_err_assert(err) \
	tokenizer_err__assert(err, __func__, __FILE__, __LINE__)

typedef struct {
	char* text;
	strv_t* lines;
	size_t cline;
	size_t nlines;

	tokenizer_err_t err;
} tokenizer_t;

tokenizer_err_t const tokenizer_check(tokenizer_t* tokenizer);
int const tokenizer_ok(tokenizer_t* tokenizer);

void tokenizer__dump(tokenizer_t* tokenizer, char const* func, char const* file,
					 size_t nline);

void tokenizer__assert(tokenizer_t* tokenizer, char const* func, char const* file,
					   size_t nline);

#define tokenizer_dump(tokenizer) \
	tokenizer__dump(tokenizer, __func__, __FILE__, __LINE__)

#define tokenizer_assert(tokenizer) \
	tokenizer__assert(tokenizer, __func__, __FILE__, __LINE__)

tokenizer_err_t const tokenizer_init(tokenizer_t* tokenizer, char const* filename);
void tokenizer_free(tokenizer_t* tokenizer);

token_t const tokenizer_ntok(tokenizer_t* tokenizer);

#endif
