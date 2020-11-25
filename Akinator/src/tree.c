#include <ttrack/dbg.h>
#include "io.h"
#include "tree.h"


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

static void tree__dump_body(node_t const* root) {
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

void tree__assert(node_t const* root, char const* funcname, char const* filename, 
				  size_t nline)
{$_
	if(tree_check(root) != TREE_ERR_OK) {
		tree__dump(root, funcname, filename, nline);
		ASSERT(0);
	}
$$
}


static void tree__traversal(node_t* node)
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

static void tree__free(node_t* root)
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


