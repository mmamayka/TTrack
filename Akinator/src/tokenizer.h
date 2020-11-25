#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <ttrack/strv.h>

// ---------- token_type_t ---------- //

typedef enum {
	TOK_UNKNOWN,
	TOK_OPENBR,
	TOK_CLOSEBR,
	TOK_STRING,
	TOK_EOF,

	TOK_NTYPES
} token_type_t;

int const token_type_ok(token_type_t type);
void token_type__assert(token_type_t type, char const* funcname, char const* filename,
						size_t nline);

#define token_type_assert(type) \
	token_type__assert(type, __func__, __FILE__, __LINE__)

char const* token_type_str(token_type_t type);

// ---------- token_t ---------- //

typedef struct {
	token_type_t type;
	strv_t data;
} token_t;

int const token_ok(token_t const* token);
void token__dump(token_t const* token, char const* funcname, char const* filename,
				 size_t nline);

#define token_dump(token) \
	token__dump(token, __func__, __FILE__, __LINE__)

void token__assert(token_t const* token, char const* funcname, char const* filename,
				   size_t nline);

#define token_assert(token) \
	token__assert(token, __func__, __FILE__, __LINE__)

token_t token_init(token_type_t type, strv_t data);
token_t token_init0(token_type_t type);

token_type_t token_type(token_t const* token);
strv_t token_data(token_t const* token);

// ---------- tokenizer_err_t ---------- //

typedef enum {
	TOKENIZER_ERR_OK,
	TOKENIZER_ERR_MEM,
	TOKENIZER_ERR_STDIO,
	TOKENIZER_ERR_NF,
	TOKENIZER_ERR_EOSTR,
	TOKENIZER_ERR_NULL,
	TOKENIZER_ERR_STATE,
	TOKENIZER_ERR_ERRANGE,
	
	TOKENIZER_NERRORS
} tokenizer_err_t;

int const tokenizer_err_ok(tokenizer_err_t err);
void tokenizer__err_assert(tokenizer_err_t err, char const* funcname, char const* filename,
						   size_t nline);

#define tokenizer_err_assert(err) \
	tokenizer__err_assert(err, __func__, __FILE__, __LINE__)

char const* tokenizer_errstr(tokenizer_err_t err);

// ---------- tokenizer_t ---------- //

typedef struct {
	char* text;

	size_t nlines;
	size_t cline;
	strv_t* lines;

	tokenizer_err_t err;
} tokenizer_t;

tokenizer_err_t const tokenizer_check(tokenizer_t* t);
int const tokenizer_ok(tokenizer_t* t);
void tokenizer__dump(tokenizer_t* t, char const* funcname, char const* filename,
					 size_t nline);

#define tokenizer_dump(t) \
	tokenizer__dump(t, __func__, __FILE__, __LINE__)

void tokenizer__assert(tokenizer_t* t, char const* funcname, char const* filename,
					   size_t nline);

#define tokenizer_assert(t) \
	tokenizer__assert(t, __func__, __FILE__, __LINE__)

tokenizer_err_t tokenizer_init(tokenizer_t* t, char const* fname);
void tokenizer_free(tokenizer_t* t);

token_t tokenizer_ntok(tokenizer_t* t);

#endif
