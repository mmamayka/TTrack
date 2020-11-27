#include <stdlib.h>
#include <ctype.h>
#include <ttrack/dbg.h>
#include <ttrack/text.h>
#include <math.h>
#include "tokenizer.h"

// ---------- token_t ----------- //

ENUMEX_SOURCE(token_type,
	"left brace",
	"right brace",
	"variable name",
	"number",
	"add",
	"sub",
	"mul",
	"div",
	"pow",
	"end of file",
	"unknown"
);

ENUMEX_SOURCE(token_err,
	"ok",
	"instance pointer is null",
	"invalid token type",
	"invalid token value",
	"invalid token name"
);


token_err_t const token_check(token_t* token)
{$_
	if(token == NULL) { RETURN(TOKEN_ERR_NULL); }
	if(!token_type_ok(token->type)) { RETURN(TOKEN_ERR_INVTYPE); }

	switch(token->type) {
	case TOKEN_NUMBER:
		if(isnan(token->value)) { RETURN(TOKEN_ERR_INVVAL); }
		break;

	default:
		break;
	}
	RETURN(TOKEN_ERR_OK);
$$
}

int const token_ok(token_t* token)
{$_
	RETURN(token_check(token) == TOKEN_ERR_OK);
$$
}

void token__dump(token_t* token, char const* funcname, char const* filename, 
				 size_t nline)
{$_
	token_err_t err = token_check(token);
	dump("token_t [%p] dump from %s (%s %i), reason %i (%s) = {\n",
		 token, funcname, filename, nline, err, token_err_str(err));

	if(token != NULL) {
		dump("\ttype = %i (%s)\n", token->type, token_type_str(token->type));

		switch(token->type) {
		case TOKEN_NUMBER:
			dump("\tvalue = %lf\n", token->value);
			break;

		case TOKEN_VARNAME:
			dump("\tname = %c\n", token->name);
			break;

		default:
			break;
		}
	}

	dump("}\n");
$$
}

void token__assert(token_t* token, char const* funcname, char const* filename,
				   size_t nline)
{$_
	if(!token_ok(token)) {
		token__dump(token, funcname, filename, nline);
		ASSERT_(!"invalid token", funcname, filename, nline);
	}
$$
}

token_t const token_triv(token_type_t type)
{$_
	token_type_assert(type);

	token_t t;
	t.type = type;

	token_assert(&t);

	RETURN(t);
$$
}

token_t const token_varname(char name)
{$_
	token_t t;
	t.type = TOKEN_VARNAME;
	t.name = name;

	token_assert(&t);

	RETURN(t);
$$
}

token_t const token_number(double value)
{$_
	ASSERT(!isnan(value));

	token_t t;
	t.type = TOKEN_NUMBER;
	t.value = value;

	token_assert(&t);

	RETURN(t);
$$
}

// ---------- tokenizer_t ---------- //

ENUMEX_SOURCE(tokenizer_err,
	"ok",
	"instance pointer is null",
	"file not found",
	"out of memory",
	"invalid state",
	"error in stdio"
);

static tokenizer_err_t tokenizer__set_error(tokenizer_t* tokenizer, 
											tokenizer_err_t err);

static tokenizer_err_t tokenizer__set_error(tokenizer_t* tokenizer, 
											tokenizer_err_t err)
{$_
	tokenizer_err_assert(err);
	ASSERT(tokenizer != NULL);

	RETURN(tokenizer->err = err);
$$
}

tokenizer_err_t const tokenizer_check(tokenizer_t* tokenizer)
{$_
	if(tokenizer == NULL) { RETURN(TOKENIZER_ERR_NULL); }
	if(tokenizer->text == NULL || tokenizer->lines == NULL ||
	   tokenizer->cline > tokenizer->nlines) {
		RETURN(tokenizer__set_error(tokenizer, TOKENIZER_ERR_STATE));
	}

	for(size_t i = 0; i < tokenizer->nlines; ++i) {
		if(!strv_ok(tokenizer->lines + i)) {
			RETURN(tokenizer__set_error(tokenizer, TOKENIZER_ERR_STATE));
		}
	}

	RETURN(tokenizer->err);
$$
}

int const tokenizer_ok(tokenizer_t* tokenizer)
{$_
	RETURN(tokenizer_check(tokenizer) == TOKENIZER_ERR_OK);
$$
}

void tokenizer__dump(tokenizer_t* tokenizer, char const* func, char const* file,
					 size_t nline)
{$_
	tokenizer_err_t err = tokenizer_check(tokenizer);
	dump("tokenizer_t [%p] dump from %s (%s %zu), reason %i (%s) = {\n",
		 tokenizer, func, file, nline, err, tokenizer_err_str(err));

	if(tokenizer != NULL) {
		dump("\ttext [%p] = \"%s\"\n", tokenizer->text, tokenizer->text);
		dump("\tnlines = %zu\n", tokenizer->nlines);
		dump("\tcline = %zu\n", tokenizer->cline);

		dump("\tlines [%p] = {\n", tokenizer->lines);

		if(tokenizer->lines != NULL) {
			for(size_t i = 0; i < tokenizer->nlines; ++i) {
				dump("\t\t%c [%zu] = \n", i < tokenizer->cline ? '*' : ' ', i);

				DUMP_DEPTH += 2;
				strv_dump(tokenizer->lines + i);
				DUMP_DEPTH -= 2;
			}
		}

		dump("\t}\n");
		dump("\terr = %i (%s)\n", tokenizer->err, 
			 tokenizer_err_ok(tokenizer->err) ? 
			 tokenizer_err_str(tokenizer->err) : "INVALID");
	}
	dump("}\n");
$$
}

void tokenizer__assert(tokenizer_t* tokenizer, char const* func, char const* file,
					   size_t nline)
{$_
	if(!tokenizer_ok(tokenizer)) {
		tokenizer__dump(tokenizer, func, file, nline);
		ASSERT_(!"invalid tokenizer", func, file, nline);
	}
$$
}

tokenizer_err_t const tokenizer_init(tokenizer_t* tokenizer, char const* filename)
{$_
	ASSERT(tokenizer != NULL);
	ASSERT(filename != NULL);

	RF_err_t rt_err = RF_OK;
	size_t text_size = 0;
	tokenizer->text = read_text2(filename, &text_size, &rt_err);

	switch(rt_err) {
	case RF_MEMORY:
		RETURN(tokenizer__set_error(tokenizer, TOKENIZER_ERR_MEM));
		break;

	case RF_STDIO:
		RETURN(tokenizer__set_error(tokenizer, TOKENIZER_ERR_STDIO));
		break;

	case RF_NOTFOUND:
		RETURN(tokenizer__set_error(tokenizer, TOKENIZER_ERR_NF));
		break;

	default:
		break;
	}

	tokenizer->lines = get_text_lines(tokenizer->text, text_size, '\n', 
									  &tokenizer->nlines);
	if(tokenizer->lines == NULL) {
		free(tokenizer->text);
		RETURN(tokenizer__set_error(tokenizer, TOKENIZER_ERR_MEM));
	}

	tokenizer->cline = 0;
	tokenizer->err = TOKENIZER_ERR_OK;

	tokenizer_assert(tokenizer);

	RETURN(TOKENIZER_ERR_OK);
$$
}

void tokenizer_free(tokenizer_t* tokenizer)
{$_
	tokenizer_assert(tokenizer);

	free(tokenizer->lines);
	free(tokenizer->text);
$$
}

token_t const tokenizer_ntok(tokenizer_t* tokenizer)
{$_
	tokenizer_assert(tokenizer);
	
	while(tokenizer->cline < tokenizer->nlines) {
		strv_chompf(tokenizer->lines + tokenizer->cline);
		if(!strv_empty(tokenizer->lines + tokenizer->cline)) {
			break;
		}
		++tokenizer->cline;
	}

	if(tokenizer->cline == tokenizer->nlines) {
		tokenizer_assert(tokenizer);
		RETURN(token_triv(TOKEN_EOF));
	}

	char front = strv_front(tokenizer->lines + tokenizer->cline);

	if(isdigit(front)) {
		char* pend;
		double value = strtod(strv_begin(tokenizer->lines + tokenizer->cline), &pend);

		tokenizer->lines[tokenizer->cline].pfirst = pend;

		RETURN(token_number(value));
	}

	if(islower(front)) {
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_varname(front));
	}

	switch(front) {
	case '(':
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_triv(TOKEN_LBRACE));
		break;

	case ')':
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_triv(TOKEN_RBRACE));
		break;

	case '+':
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_triv(TOKEN_ADD));
		break;

	case '-':
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_triv(TOKEN_SUB));
		break;

	case '*':
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_triv(TOKEN_MUL));
		break;

	case '/':
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_triv(TOKEN_DIV));
		break;

	case '^':
		++tokenizer->lines[tokenizer->cline].pfirst;
		RETURN(token_triv(TOKEN_POW));
		break;
	
	default:
		RETURN(token_triv(TOKEN_UNKNOWN));
	
	}
	
$$
}


