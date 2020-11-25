#include <stdlib.h>
#include <ttrack/dbg.h>
#include <ttrack/text.h>
#include <string.h>
#include "tokenizer.h"

// ---------- token_type_t ---------- //

int const token_type_ok(token_type_t type)
{$_
	RETURN(type >= 0 && type < TOK_NTYPES);
$$
}

void token_type__assert(token_type_t type, char const* funcname, char const* filename,
						size_t nline)
{$_
	if(!token_type_ok(type)) {
		ASSERT_(!"invalid token type", funcname, filename, nline);
	}
$$
}

char const* token_type_str(token_type_t type)
{$_
	token_type_assert(type);

	char const* TABLE[TOK_NTYPES] = {
		"unknown",
		"left brace",
		"right brace",
		"string",
		"end of file"
	};
	RETURN(TABLE[type]);
$$
}

// ---------- token_t ---------- //

int const token_ok(token_t const* token)
{$_
	RETURN(token != NULL && strv_ok(&token->data) && token_type_ok(token->type));
$$
}

void token__dump(token_t const* token, char const* funcname, char const* filename,
				 size_t nline)
{$_
	int ok = token_ok(token);
	dump("token_t [%p] dump from %s (%s %zu), reason %i (%s) = {\n",
		 token, funcname, filename, nline, ok, ok ? "ok" : "not ok");

	if(token != NULL) {
		dump("\ttype = %i (%s)\n", token->type, token_type_str(token->type));
		dump("t\tdata = \n");

		DUMP_DEPTH += 1;
		strv_dump(&token->data);
		DUMP_DEPTH -= 1;
	}

	dump("}\n");
$$
}


void token__assert(token_t const* token, char const* funcname, char const* filename,
				   size_t nline)
{$_
	if(!token_ok(token)) {
		token__dump(token, funcname, filename, nline);
		ASSERT_(!"invalid token", funcname, filename, nline);
	}
$$}


token_t token_init(token_type_t type, strv_t data)
{$_
	token_type_assert(type);
	strv_assert(&data);

	token_t tok = { type, data };
	token_assert(&tok);

	RETURN(tok);
$$
}

token_t token_init0(token_type_t type)
{$_
	token_type_assert(type);

	token_t tok = { type, strv_make("") };
	token_assert(&tok);

	RETURN(tok);
$$
}

token_type_t token_type(token_t const* token) 
{$_
	token_assert(token);
	RETURN(token->type);
$$
}

strv_t token_data(token_t const* token) 
{$_
	token_assert(token);
	RETURN(token->data);
$$
}

// ---------- tokenizer_err_t ---------- //

int const tokenizer_err_ok(tokenizer_err_t err)
{$_
	RETURN(err >= 0 && err < TOKENIZER_NERRORS);
$$
}

void tokenizer__err_assert(tokenizer_err_t err, char const* funcname, char const* filename,
						   size_t nline)
{$_
	ASSERT_(tokenizer_err_ok(err), funcname, filename, nline);
$$
}


char const* tokenizer_errstr(tokenizer_err_t err)
{$_
	tokenizer_err_assert(err);

	char const* TABLE[TOKENIZER_NERRORS] = {
		"ok",
		"out of memory",
		"error in stdio",
		"file not found",
		"unexpected end of line",
		"instance pointer is null",
		"invalid state",
		"invalid error"
	};
	RETURN(TABLE[err]);
$$
}

// ---------- tokenizer_t ---------- //

static tokenizer_err_t tokenizer__set_error(tokenizer_t* t, tokenizer_err_t e);

static tokenizer_err_t tokenizer__set_error(tokenizer_t* t, tokenizer_err_t e) 
{$_
	tokenizer_err_assert(e);
	RETURN(t->err = e);
$$
}

tokenizer_err_t const tokenizer_check(tokenizer_t* t) 
{$_
	if(t == NULL) {
		RETURN(tokenizer__set_error(t, TOKENIZER_ERR_NULL));
	}
	if(t->text == NULL || t->lines == NULL || t->cline > t->nlines) {
		RETURN(tokenizer__set_error(t, TOKENIZER_ERR_STATE));
	}

	for(size_t i = 0; i < t->nlines; ++i) {
		if(!strv_ok(t->lines + i)) {
			RETURN(tokenizer__set_error(t, TOKENIZER_ERR_STATE));
		}
	}
	RETURN(TOKENIZER_ERR_OK);
$$
}

int const tokenizer_ok(tokenizer_t* t) 
{$_
	RETURN(tokenizer_check(t) == TOKENIZER_ERR_OK);
$$
}

void tokenizer__dump(tokenizer_t* t, char const* funcname, char const* filename,
					 size_t nline)
{$_
	tokenizer_err_t err = tokenizer_check(t);
	dump("tokenizer_t [%p] dump from %s (%s %zu), reason %i (%s) = {\n",
		 t, funcname, filename, nline, err, tokenizer_errstr(err));

	if(t != NULL) {
		dump("\ttext [%p] = \"%s\"\n", t->text, t->text);
		dump("\tnlines = %zu\n", t->nlines);
		dump("\tcline = %zu\n", t->cline);

		dump("\tlines [%p] = {\n", t->lines);

		for(size_t i = 0; i < t->nlines; ++i) {
			dump("\t\t%c [%zu] = \n", i < t->cline ? '*' : ' ', i);

			DUMP_DEPTH += 2;
			strv_dump(t->lines + i);
			DUMP_DEPTH -= 2;
		}

		dump("\t}\n");
		dump("\terr = %i (%s)\n", t->err, 
			 tokenizer_err_ok(t->err) ? tokenizer_errstr(t->err) : "INVALID");
	}
	dump("}\n");
$$
}

void tokenizer__assert(tokenizer_t* t, char const* funcname, char const* filename,
					   size_t nline)
{$_
	if(!tokenizer_ok(t)) {
		tokenizer__dump(t, funcname, filename, nline);
		ASSERT_(!"invalid tokenizer", funcname, filename, nline);
	}
$$
}

tokenizer_err_t tokenizer_init(tokenizer_t* t, char const* fname) 
{$_
	ASSERT(t != NULL);
	ASSERT(fname != NULL);

	RF_err_t rt_err = RF_OK;
	size_t text_size = 0;
	t->text = read_text2(fname, &text_size, &rt_err);

	switch(rt_err) {
	case RF_MEMORY:
		RETURN(tokenizer__set_error(t, TOKENIZER_ERR_MEM));
		break;

	case RF_STDIO:
		RETURN(tokenizer__set_error(t, TOKENIZER_ERR_STDIO));
		break;

	case RF_NOTFOUND:
		RETURN(tokenizer__set_error(t, TOKENIZER_ERR_NF));
		break;

	default:
		break;
	}

	t->lines = get_text_lines(t->text, text_size, '\n', &t->nlines);
	if(t->lines == NULL) {
		free(t->text);
		RETURN(tokenizer__set_error(t, TOKENIZER_ERR_MEM));
	}

	t->cline = 0;
	t->err = TOKENIZER_ERR_OK;

	tokenizer_assert(t);

	RETURN(TOKENIZER_ERR_OK);
$$
}

void tokenizer_free(tokenizer_t* t) 
{$_
	tokenizer_assert(t);

	free(t->lines);
	free(t->text);
$$
}

token_t tokenizer__mkstr(tokenizer_t* t) 
{$_
	tokenizer_assert(t);

	char const* pos = memchr(t->lines[t->cline].pfirst + 1, '\"', // TODO
							 strv_len(&t->lines[t->cline]) - 1);

	if(pos == NULL) {
		tokenizer__set_error(t, TOKENIZER_ERR_EOSTR);
		RETURN(token_init0(TOK_UNKNOWN));
	}

	token_t tok = token_init(TOK_STRING, 
								 strv_init(t->lines[t->cline].pfirst + 1, pos)); // TODO

	t->lines[t->cline].pfirst = pos + 1; // TODO

	tokenizer_assert(t);

	RETURN(tok);
$$
}

token_t tokenizer_ntok(tokenizer_t* t) 
{$_
	tokenizer_assert(t);

	while(t->cline < t->nlines) {
		strv_chompf(t->lines + t->cline);
		if(!strv_empty(t->lines + t->cline)) {
			break;
		}
		++t->cline;
	}

	if(t->cline == t->nlines) {
		tokenizer_assert(t);
		RETURN(token_init0(TOK_EOF));
	}

	switch(strv_front(&t->lines[t->cline])) {
	case '[':
		++(t->lines[t->cline].pfirst); // TODO
		tokenizer_assert(t);
		RETURN(token_init0(TOK_OPENBR));
		break;

	case ']':
		++(t->lines[t->cline].pfirst); // TODO
		tokenizer_assert(t);
		RETURN(token_init0(TOK_CLOSEBR));
		break;

	case '\"':
		tokenizer_assert(t);
		RETURN(tokenizer__mkstr(t));
		break;

	default:
		tokenizer_assert(t);
		RETURN(token_init0(TOK_UNKNOWN));
		break;
	}
$$
}

