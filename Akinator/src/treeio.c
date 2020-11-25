#include <stdlib.h>
#include <ttrack/dbg.h>
#include "tokenizer.h"
#include "treeio.h"
#include "tree.h"

static void tree__write_indent(FILE* stream, size_t depth);
static void tree__write(node_t* root, FILE* stream, size_t depth);
static node_t* tree__read(tokenizer_t* tokenizer);

static void tree__write_indent(FILE* stream, size_t depth) 
{$_
	for(size_t i = 0; i < depth; ++i) { fputc('\t', stream); }
$$
}

static void tree__write(node_t* root, FILE* stream, size_t depth) 
{$_
	tree__write_indent(stream, depth);
	fputs("[\n", stream);

	tree__write_indent(stream, depth);
	fprintf(stream, "\t\"%s\"\n", root->data);

	if(root->lch != NULL) {
		tree__write(root->lch, stream, depth + 1);
	}
	if(root->rch != NULL) {
		tree__write(root->rch, stream, depth + 1);
	}

	tree__write_indent(stream, depth);
	fputs("]\n", stream);
$$
}

void tree_write(node_t* root, FILE* stream) 
{$_
	tree_assert(root);
	stream_assert(stream);
	tree__write(root, stream, 0);
$$
}

void tree_write_file(node_t* root, char const* fname) 
{$_
	tree_assert(root);
	ASSERT(fname != NULL);

	FILE* f = fopen(fname, "w");
	tree__write(root, f, 0);
	fclose(f);
}

static node_t* tree__read(tokenizer_t* tokenizer)
{$_
	tokenizer_assert(tokenizer);

	node_t* node = (node_t*)calloc(sizeof(node_t), 1);
	if(node == NULL) {
		RETURN(NULL);
	}

	for(token_t t = tokenizer_ntok(tokenizer); t.type != TOK_CLOSEBR;
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

	token_t tok = tokenizer_ntok(&t);
	if(tok.type != TOK_OPENBR) {
		tokenizer_free(&t);
		RETURN(NULL);
	}

	node_t* root = tree__read(&t);

	tokenizer_free(&t);

	RETURN(root);
$$
}


