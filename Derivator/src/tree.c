#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ttrack/dbg.h>
#include "tree.h"

ENUMEX_SOURCE(tree_parse_err,
	"ok",
	"out of memory",
	"unexected end of file",
	"invalid name",
	"invalid value",
	"unexected token",
	"unknown token"
);

ENUMEX_SOURCE(tree_err,
	"ok",
	"instance pointer is null",
	"error in stdio",
	"loop in the tree",
	"invalid node"
);

tree_parse_err_t TREE_PARSE_ERR = TREE_PARSE_ERR_OK;


static tree_err_t const tree__check(node_t* node);
static void tree__dump_body(node_t* node);

static void tree__free(node_t* root);
static node_t* tree__parse(tokenizer_t* tokenizer, vardic_t* vardic);

static tree_err_t const tree__check(node_t* node) 
{$_
	if(node == NULL) { RETURN( TREE_ERR_OK ); }
	if(node->color == 1) { RETURN( TREE_ERR_LOOP ); }
	if(!node_ok(node)) { RETURN(TREE_ERR_NODE); }

	node->color = 1;

	tree_err_t err1 = tree__check(node->lch);
	tree_err_t err2 = tree__check(node->rch);

	node->color = 0;

	RETURN(err1 == TREE_ERR_OK ? err2 : err1);
$$
}

tree_err_t const tree_check(node_t* node)
{$_
	if(node == NULL) { RETURN(TREE_ERR_NULL); }
	RETURN(tree__check(node));
$$
}
int const tree_ok(node_t* node)
{$_
	RETURN(tree_check(node) == TREE_ERR_OK);
$$
}

static void tree__dump_body(node_t* node) 
{$_
	node_dump(node);

	if(node != NULL) {
		if(node->color) {
			dump("LOOP FOUND\n");
		}

		node->color = 1;

		node_dump(node->lch);
		node_dump(node->rch);

		node->color = 0;
	}
$$
}
void tree__dump(node_t* node, char const* func, char const* file, size_t nline)
{$_
	tree_err_t err = tree_check(node);
	dump("tree [%p] dump from %s (%s %zu), reason %i (%s) = {\n",
		 node, func, file, nline, err, tree_err_str(err));

	DUMP_DEPTH += 1;
	tree__dump_body(node);
	DUMP_DEPTH -= 1;
$$
}

void tree__assert(node_t* node, char const* func, char const* file, size_t nline)
{$_
	if(!tree_ok(node)) {
		tree__dump(node, func, file, nline);
		ASSERT_(!"invalid tree", func, file, nline);
	}
$$
}

static void tree__free(node_t* root) 
{$_
	if(root != NULL) {
		tree__free(root->lch);
		tree__free(root->rch);
		free(root);
	}
$$
}

void tree_free(node_t* root) 
{$_
	tree__free(root);
$$
}

static node_t* tree__parse(tokenizer_t* tokenizer, vardic_t* vardic)
{$_
	tokenizer_assert(tokenizer);
	vardic_assert(vardic);

	node_t* chnode = NULL;
	node_t* node = (node_t*)calloc(1, sizeof(node_t));
	if(node == NULL) {
		TREE_PARSE_ERR = TREE_PARSE_ERR_MEM;
		RETURN(NULL);
	}

	for(;;) {
		token_t t = tokenizer_ntok(tokenizer);

		if(t.type == TOKEN_RBRACE) { 
			break;
		}

		switch(t.type) {
		case TOKEN_NUMBER:
			if(isnan(t.value)) {
				TREE_PARSE_ERR = TREE_PARSE_ERR_VAL;
				goto err;
			}

			node->type = NODE_TYPE_CONST;
			node->value = t.value;
			break;

		case TOKEN_VARNAME:
			if(!vardic_name_ok(vardic, t.name)) {
				TREE_PARSE_ERR = TREE_PARSE_ERR_NAME;
				goto err;
			}

			node->type = NODE_TYPE_VAR;
			node->varname = t.name;
			break;

		case TOKEN_LBRACE:
			chnode = tree__parse(tokenizer, vardic);
			if(chnode == NULL) {
				goto err;
			}

			if(node->lch == NULL) {
				node->lch = chnode;
			}
			else if(node->rch == NULL) {
				node->rch = chnode;
			}
			else {
				TREE_PARSE_ERR = TREE_PARSE_ERR_UNEXP;
				goto err;
			}
			break;

		case TOKEN_ADD:
			node->type = NODE_TYPE_OP;
			node->optype = OP_ADD;
			break;

		case TOKEN_SUB:
			node->type = NODE_TYPE_OP;
			node->optype = OP_SUB;
			break;

		case TOKEN_MUL:
			node->type = NODE_TYPE_OP;
			node->optype = OP_MUL;
			break;

		case TOKEN_DIV:
			node->type = NODE_TYPE_OP;
			node->optype = OP_DIV;
			break;

		case TOKEN_POW:
			node->type = NODE_TYPE_OP;
			node->optype = OP_POW;
			break;

		case TOKEN_EOF:
			TREE_PARSE_ERR = TREE_PARSE_ERR_EOF;
			goto err;
			break;

		case TOKEN_UNKNOWN:
			TREE_PARSE_ERR = TREE_PARSE_ERR_UNKNOWN;
			goto err;
			break;

		default:
			break;

		}
	}

	node_assert(node);
	RETURN(node);

err:
	if(node->lch != NULL) { tree_free(node->lch); }
	if(node->rch != NULL) { tree_free(node->rch); }

	free(node);
	RETURN(NULL);
$$
}

size_t tree__todot(node_t* root, size_t id, FILE* stream)
{$_
	if(root == NULL) { RETURN(id); }

	switch(root->type) {
	case NODE_TYPE_CONST:
		fprintf(stream, "\t%zu [label=\"%lf\"];\n", id, root->value);
		break;
	
	case NODE_TYPE_VAR:
		fprintf(stream, "\t%zu [label=\"%c\"];\n", id, root->varname);
		break;
	
	case NODE_TYPE_OP:
		fprintf(stream, "\t%zu [label=\"%s\"];\n", id, op_type_str(root->optype));
		break;

	default:
		break;
	}

	size_t nid = id;
	if(root->lch != NULL) {
		fprintf(stream, "\t%zu -> %zu [label=\"L\"];\n", id, id + 1);
		nid = tree__todot(root->lch, id + 1, stream);
	}
	if(root->rch != NULL) {
		fprintf(stream, "\t%zu -> %zu [label=\"R\"];\n", id, nid + 1);
		nid = tree__todot(root->rch, nid + 1, stream);
	}

	RETURN(nid + 1);
$$
}

tree_err_t const tree_todot(node_t* root, FILE* stream)
{$_
	tree_assert(root);
	stream_assert(stream);

	fprintf(stream, "digraph G {\n");
	tree__todot(root, 0, stream);
	fprintf(stream, "}\n");

	RETURN(TREE_ERR_OK);
$$
}

node_t* tree_parse(tokenizer_t* tokenizer, vardic_t* vardic)
{$_
	tokenizer_assert(tokenizer);
	vardic_assert(vardic);

	TREE_PARSE_ERR = TREE_PARSE_ERR_OK;

	token_t t = tokenizer_ntok(tokenizer);
	if(t.type != TOKEN_LBRACE) {
		TREE_PARSE_ERR = TREE_PARSE_ERR_UNEXP;
		RETURN(NULL);
	}

	RETURN(tree__parse(tokenizer, vardic));
$$
}

node_t* tree__copy(node_t* root) 
{$_
	node_t* croot = (node_t*)calloc(1, sizeof(node_t));
	if(croot == NULL) {
		RETURN(NULL);
	}
	memcpy(croot, root, sizeof(node_t));

	if(root->lch != NULL) {
		node_t* chroot = tree__copy(root->lch);
		if(chroot == NULL) {
			free(croot);
			RETURN(NULL);
		}

		croot->lch = chroot;
	}
	
	if(root->rch != NULL) {
		node_t* chroot = tree__copy(root->rch);
		if(chroot == NULL) {
			free(croot);
			RETURN(NULL);
		}

		croot->rch = chroot;
	}

	RETURN(croot);
$$
}

node_t* tree_copy(node_t* root)
{$_
	tree_assert(root);
	RETURN(tree__copy(root));
$$
}


double const tree__eval(node_t* root, vardic_t* vardic)
{$_
	double val;

	switch(root->type) {
	case NODE_TYPE_CONST:
		RETURN(root->value);
		break;

	case NODE_TYPE_VAR:
		vardic_get(vardic, root->varname, &val);
		RETURN(val);
		break;
	
	case NODE_TYPE_OP:
		switch(root->optype) {
		case OP_ADD:
			RETURN(tree__eval(root->lch, vardic) + tree__eval(root->rch, vardic));
			break;

		case OP_SUB:
			RETURN(tree__eval(root->lch, vardic) - tree__eval(root->rch, vardic));
			break;

		case OP_MUL:
			RETURN(tree__eval(root->lch, vardic) * tree__eval(root->rch, vardic));
			break;

		case OP_DIV:
			RETURN(tree__eval(root->lch, vardic) / tree__eval(root->rch, vardic));
			break;

		case OP_POW:
			RETURN(pow(tree__eval(root->lch, vardic), tree__eval(root->rch, vardic)));
			break;

		default:
			break;
		}
	
	default:
		break;
	}

	RETURN(NAN);
$$
}

double const tree_eval(node_t* root, vardic_t* vardic)
{$_
	tree_assert(root);
	vardic_assert(vardic);
	RETURN(tree__eval(root, vardic));
$$
}

int const tree__is_const(node_t* root)
{$_
	switch(root->type) {
	case NODE_TYPE_CONST:
		RETURN(1);
		break;

	case NODE_TYPE_VAR:
		RETURN(0);
		break;

	case NODE_TYPE_OP:
		RETURN(tree__is_const(root->lch) && tree__is_const(root->rch));
		break;
	
	default:
		break;
	}
	RETURN(0);
$$
}

int const tree_is_const(node_t* root)
{$_
	tree_assert(root);
	RETURN(tree__is_const(root));
$$
}
