#include <ttrack/dbg.h>
#include <ttrack/text.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

static int const tokenizer__error_ok(tokenizer_err_t errc) {
	return errc >= 0 && errc < TOKENIZER_NERRORS;
}

char const* tokenizer_errstr(tokenizer_err_t errc)
{$_
	ASSERT(tokenizer__error_ok(errc));

	char const* TABLE[] = {
		"ok",
		"out of memory",
		"stdio internal error",
		"invalid state",
		"end of file reached",
		"too many arguments"
	};
	RETURN(TABLE[errc]);
}

typedef struct {
	tokenizer_err_t err;
	char* text;
	strv_t* lines;
	size_t nlines;
	size_t curline;
} tokenizer_t;

tokenizer_t tokenizer = {};

static tokenizer_err_t tokenizer__set_error(tokenizer_err_t err)
{$_
	return tokenizer.err = err;
}

static void tokenizer__remove_comments() 
{$_
	for(strv_t* line = tokenizer.lines; line < tokenizer.lines + tokenizer.nlines;
		++line)
	{
		char* pos = memchr(strv_begin(line), ';', strv_len(line));
		if(pos != NULL) {
			line->plast = pos;
			*(char*)line->plast = '\0'; // <- a bit shitty...
		}
	}
$$
}

tokenizer_err_t const tokenizer_init(FILE* file)
{$_
	stream_assert(file);

	RT_err_t rt_errc;
	size_t text_size;
	tokenizer.text = read_text(file, &text_size, &rt_errc);
	switch(rt_errc) {
	case RT_MEMORY:
		RETURN(tokenizer__set_error(TOKENIZER_ERR_MEM));
		break;
	case RT_IO:
		RETURN(tokenizer__set_error(TOKENIZER_ERR_STDIO));
		break;
	default:
		break;
	}

	tokenizer.lines = get_text_lines(tokenizer.text, text_size, '\n', &tokenizer.nlines);
	if(tokenizer.lines == NULL) {
		free(tokenizer.text);
		RETURN(tokenizer__set_error(TOKENIZER_ERR_MEM));
	}

	for(size_t i = 0; i < tokenizer.nlines - 1; ++i) {
		*(char*)(tokenizer.lines[i].plast) = '\0';
	}

	tokenizer.curline = 0;
	tokenizer__remove_comments();
	RETURN(TOKENIZER_ERR_OK);
}

void tokenizer_free()
{$_
	tokenizer_assert();

	free(tokenizer.lines);
	free(tokenizer.text);
$$
}

tokenizer_err_t const tokenizer_nextline(size_t max_args, char* args[], size_t* num) 
{$_
	ASSERT(args != NULL);
	ASSERT(num != NULL);

	char const* SPACES = " \t";

	if(tokenizer.curline == tokenizer.nlines) {
		RETURN(tokenizer__set_error(TOKENIZER_ERR_EOF));
	}

	strv_t* line = tokenizer.lines + tokenizer.curline;

	size_t nargs = 0;
	char* pos = strtok((char*)line->pfirst, SPACES);

	while(pos != NULL && nargs < max_args) {
		args[nargs++] = pos;
		pos = strtok(NULL, SPACES);
	}

	*num = nargs;
	++tokenizer.curline;

	if(pos != NULL) {
		RETURN(tokenizer__set_error(TOKENIZER_ERR_OVERFLOW));
	}
	
	RETURN(TOKENIZER_ERR_OK);
}

size_t const tokenizer_nline()
{$_
	tokenizer_assert();
	RETURN(tokenizer.curline);
}

tokenizer_err_t const tokenizer_error()
{$_
	RETURN(tokenizer.err);
$$
}
void tokenizer_clear_error()
{$_
	tokenizer.err = TOKENIZER_ERR_OK;
$$
}

tokenizer_err_t const tokenizer_check() 
{$_
	if(tokenizer.text == NULL || tokenizer.lines == NULL || 
	   tokenizer.curline > tokenizer.nlines) { 
		RETURN(tokenizer__set_error(TOKENIZER_ERR_STATE)); 
	}
	if(tokenizer.err != TOKENIZER_ERR_OK) {
		return tokenizer.err;
	}
	RETURN(TOKENIZER_ERR_OK);
}

void tokenizer__dump(char const* funcname, char const* filename, size_t nline)
{$_
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	dump("tokenizer_t dump from %s (%s %zu), reason %i (%s)\n", 
		funcname, filename, nline, tokenizer.err, tokenizer_errstr(tokenizer.err));
	dump("{\n");

	dump("\terr = %i (%s)\n", tokenizer.err, tokenizer_errstr(tokenizer.err));
	dump("\ttext [%p]\n", tokenizer.text);
	dump("\tlines [%p]\n", tokenizer.lines);
	dump("\tnlines = %zu\n", tokenizer.nlines);
	dump("\tcurline = %zu\n", tokenizer.curline);

	dump("}\n");
$$
}

void tokenizer__assert(char const* funcname, char const* filename, size_t nline)
{$_
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	if(tokenizer_check() != TOKENIZER_ERR_OK) {
		dump("tokenizer_assert failed at %s (%s %zu)\n", funcname, filename, nline);
		tokenizer__dump(funcname, filename, nline);
		ASSERT(0);
	}
$$
}



