#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ttrack/dbg.h>
#include <ttrack/binbuf.h>

#include "node.h"
#include "io.h"

typedef enum {
	TREE_ERR_OK,
	TREE_ERR_LOOP,
	TREE_ERR_NODE,
	TREE_ERR_NULL,

	TREE_NERRORS
} tree_err_t;

char const* tree_errstr(tree_err_t err)
{$_
	ASSERT(err >= 0 && err < TREE_NERRORS);

	char const* TABLE[TREE_NERRORS] = {
		"ok",
		"loop",

	};

	RETURN(TABLE[err]);
$$
}

tree_err_t tree_check(node_t const* root)
{$_
	node_err_t node_err = node_check(root);
	if(node_err != NODE_ERR_OK) {
		switch(node_err) {
		case NODE_ERR_LOOP:
			RETURN(TREE_ERR_LOOP);
			break;

		default:
			RETURN(TREE_ERR_NODE);
			break;
		}
	}

	if(root->lch == NULL) {
		RETURN(TREE_ERR_OK);
	}

	((node_t*)root)->color = 1;

	tree_err_t lerr = tree_check(root->lch);
	if(lerr != TREE_ERR_OK) {
		((node_t*)root)->color = 0;
		RETURN(lerr);
	}

	tree_err_t rerr = tree_check(root->rch);
	if(rerr != TREE_ERR_OK) {
		((node_t*)root)->color = 0;
		RETURN(rerr);
	}
	
	((node_t*)root)->color = 0;

	RETURN(TREE_ERR_OK);
}

void tree__dump_body(node_t const* root) {
	if(root != NULL) {
		node_dump(root);
		if(((node_t*)root)->color == 1) {
			dump("cycle found\n");
		}
		else {
			((node_t*)root)->color = 1;
			tree__dump_body(root->lch);
			tree__dump_body(root->rch);
			((node_t*)root)->color = 0;
		}
	}

}

void tree__dump(node_t const* root, char const* funcname, char const* filename,
				size_t nline)
{$_
	tree_err_t err = tree_check(root);
	dump("tree dump from %s (%s %i), reason %i (%s) {\n",
		 funcname, filename, nline, err, tree_errstr(err));

	DUMP_DEPTH += 1;
	tree__dump_body(root);
	DUMP_DEPTH -= 1;

	dump("}\n");
$$
}

#define tree_dump(root) \
	tree__dump(root, __func__, __FILE__, __LINE__)

void tree__assert(node_t const* root, char const* funcname, char const* filename, 
				  size_t nline)
{$_
	if(tree_check(root) != TREE_ERR_OK) {
		tree__dump(root, funcname, filename, nline);
		ASSERT(0);
	}
$$
}

#define tree_assert(root) \
	tree__assert(root, __func__, __FILE__, __LINE__)

void tree__traversal(node_t* node)
{$_
	if(node->lch == NULL) {
		printf("Is it %s?\n", node->data);
		fflush(stdout);

		int answer = yes_or_no();

		if(answer == 0) {
			node_t* node_answ = node_read_answer();
			node_t* node_question = node_read_question(NULL, NULL);

			node_question->lch = node_question;
			node_question->rch = node_answ;

			node_swap(node_question, node);
		}
		else {
			printf("That\'s cool!\n");
			fflush(stdout);
		}
	}
	else {
		printf("%s\n", node->data);
		fflush(stdout);

		int answer = yes_or_no();
		
		if(answer == 0) {
			tree__traversal(node->lch);
		}
		else {
			tree__traversal(node->rch);
		}
	}
$$
}

void tree_traversal(node_t* root)
{$_
	tree_assert(root);
	tree__traversal(root);
	tree_assert(root);
$$
}

void tree__free(node_t* root)
{$_
	if(root != NULL) {
		tree__free(root->lch);
		tree__free(root->rch);
		node_free(root);
	}
$$
}

void tree_free(node_t* root)
{$_
	tree_assert(root);
	tree__free(root);
$$
}

int const tree__write(node_t const* root) 
{$_
	if(root == NULL) {
		size_t zero = 0;
		RETURN( binbuf_write_value(size_t, zero) == BINBUF_ERR_OK );
	}
	else {
		size_t len = strlen(root->data);

		if(binbuf_write_value(size_t, len) == BINBUF_ERR_OK &&
		   binbuf_write(root->data, len) == BINBUF_ERR_OK) {
			RETURN( tree__write(root->lch) && tree__write(root->rch) );
		}
		else {
			RETURN(0);
		}
	}
$$
}

int const tree_write(node_t const* root, FILE* stream)
{$_
	tree_assert(root);
	stream_assert(stream);

	if(tree__write(root) && binbuf_flushh(stream) == BINBUF_ERR_OK) {
		binbuf_free();
		RETURN(1);
	}

	RETURN(0);
$$
}

int main() {
	node_t* root = node_make("dog", NULL, NULL);
	while(1) {
		printf("Do you want to play?\n");
		int answer = yes_or_no();

		if(!answer)
			break;

		tree_traversal(root);
	}

	tree_free(root);
}
