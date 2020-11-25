#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <ttrack/dbg.h>
#include <ttrack/binbuf.h>
#include <ttrack/strv.h>
#include <ttrack/text.h>

#include "tree.h"
#include "writer.h"
#include "io.h"

typedef enum {
	TOK_UNKNOWN,
	TOK_OPENBR,
	TOK_CLOSEBR,
	TOK_STRING,
	TOK_EOF,
} tok_type_t;

typedef struct {
	tok_type_t type;
	strv_t data;
} tok_t;

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

#define tokenizer_err_assert(err) \
	tokenizer__err_assert(err, __func__, __FILE__, __LINE__)

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

typedef struct {
	char* text;

	size_t nlines;
	size_t cline;
	strv_t* lines;

	tokenizer_err_t err;
} tokenizer_t;

tokenizer_err_t tokenizer__set_error(tokenizer_t* t, tokenizer_err_t e) 
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

#define tokenizer_dump(t) \
	tokenizer__dump(t, __func__, __FILE__, __LINE__)

void tokenizer__assert(tokenizer_t* t, char const* funcname, char const* filename,
					   size_t nline)
{$_
	if(!tokenizer_ok(t)) {
		tokenizer__dump(t, funcname, filename, nline);
		ASSERT_(!"invalid tokenizer", funcname, filename, nline);
	}
$$
}

#define tokenizer_assert(t) \
	tokenizer__assert(t, __func__, __FILE__, __LINE__)

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

tok_t tokenizer__mktok(tok_type_t type, strv_t data) 
{$_
	strv_assert(&data);

	tok_t t = { type, data };
	RETURN(t);
$$
}

tok_t tokenizer__mkstr(tokenizer_t* t) 
{$_
	tokenizer_assert(t);

	char const* pos = memchr(t->lines[t->cline].pfirst + 1, '\"', // TODO
							 strv_len(&t->lines[t->cline]) - 1);

	if(pos == NULL) {
		tokenizer__set_error(t, TOKENIZER_ERR_EOSTR);
		RETURN(tokenizer__mktok(TOK_UNKNOWN, strv_make("")));
	}

	tok_t tok = tokenizer__mktok(TOK_STRING, 
								 strv_init(t->lines[t->cline].pfirst + 1, pos)); // TODO

	t->lines[t->cline].pfirst = pos + 1;

	tokenizer_assert(t);

	RETURN(tok);
$$
}

tok_t tokenizer_ntok(tokenizer_t* t) 
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
		RETURN(tokenizer__mktok(TOK_EOF, strv_make("")));
	}

	switch(strv_front(&t->lines[t->cline])) {
	case '[':
		++(t->lines[t->cline].pfirst); // TODO
		tokenizer_assert(t);
		RETURN(tokenizer__mktok(TOK_OPENBR, strv_make("")));
		break;

	case ']':
		++(t->lines[t->cline].pfirst); // TODO
		tokenizer_assert(t);
		RETURN(tokenizer__mktok(TOK_CLOSEBR, strv_make("")));
		break;

	case '\"':
		tokenizer_assert(t);
		RETURN(tokenizer__mkstr(t));
		break;

	default:
		tokenizer_assert(t);
		RETURN(tokenizer__mktok(TOK_UNKNOWN, strv_make("")));
		break;
	}
$$
}

node_t* tree__read(tokenizer_t* tokenizer)
{$_
	tokenizer_assert(tokenizer);

	node_t* node = (node_t*)calloc(sizeof(node_t), 1);
	if(node == NULL) {
		RETURN(NULL);
	}

	for(tok_t t = tokenizer_ntok(tokenizer); t.type != TOK_CLOSEBR;
		t = tokenizer_ntok(tokenizer)) 
	{
		node_t* chnode = NULL;

		switch(t.type) {
		case TOK_OPENBR:
			chnode = tree__read(tokenizer);
			if(chnode == NULL) {
				free(node);
				RETURN(NULL);
			}

			if(node->lch == NULL) { node->lch = chnode; }
			else {node->rch = chnode; }
			break;

		case TOK_STRING:
			node->data = strv_tostr(&t.data);
			break;

		default:
			fprintf(stderr, "SHIIIT!!!! %i\n", t.type);
			free(node);
			RETURN(NULL);
			break;
		}
	}
	if(node_check(node) != NODE_ERR_OK) {
		free(node);
		RETURN(NULL);
	}
	RETURN(node);
$$
}

node_t* tree_read(char const* fname)
{$_
	tokenizer_t t;
	tokenizer_err_t err = tokenizer_init(&t, fname);
	if(err != TOKENIZER_ERR_OK) {
		RETURN(NULL);
	}

	tokenizer_ntok(&t);

	node_t* root = tree__read(&t);
	fprintf(stderr, "!!!!\n");
	tokenizer_free(&t);

	RETURN(root);
$$
}

void signal_sigsegv(int x) {
	(void)x;

	stacktrace_print(stderr);
	abort();
}

int main() 
{$_
	signal(SIGSEGV, signal_sigsegv);

	node_t* root = tree_read("database.txt");
	if(root == NULL) {
		RETURN(0);
	}

	while(1) {
		printf("Do you want to play?\n");
		int answer = yes_or_no();

		if(!answer)
			break;

		tree_traversal(root);
	}

	tree_write_file(root, "database.txt");

	tree_free(root);
$$
}
