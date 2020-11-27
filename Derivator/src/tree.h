#ifndef TREE_H
#define TREE_H

#include "enumex.h"
#include "node.h"
#include "tokenizer.h"
#include "vardic.h"

ENUMEX_HEADER(tree_parse_err,
	TREE_PARSE_ERR_OK,
	TREE_PARSE_ERR_MEM,
	TREE_PARSE_ERR_EOF,
	TREE_PARSE_ERR_NAME,
	TREE_PARSE_ERR_VAL,
	TREE_PARSE_ERR_UNEXP,
	TREE_PARSE_ERR_UNKNOWN
);

#define tree_parse_err_assert(err) \
	tree_parse_err__assert(err, __func__, __FILE__, __LINE__)

ENUMEX_HEADER(tree_err,
	TREE_ERR_OK,
	TREE_ERR_NULL,
	TREE_ERR_STDIO,
	TREE_ERR_LOOP,
	TREE_ERR_NODE
);

#define tree_err_assert(err) \
	tree_err__assert(err, __func__, __FILE__, __LINE__)

extern tree_parse_err_t TREE_PARSE_ERR;


tree_err_t const tree_check(node_t* node);
int const tree_ok(node_t* node);
void tree__dump(node_t* node, char const* func, char const* file, size_t nline);

#define tree_dump(node) \
	tree__dump(node, __func__, __FILE__, __LINE__)

void tree__assert(node_t* node, char const* func, char const* file, size_t nline);

#define tree_assert(node) \
	tree__assert(node, __func__, __FILE__, __LINE__)

tree_err_t const tree_todot(node_t* root, FILE* stream);

node_t* tree_parse(tokenizer_t* tokenizer, vardic_t* vardic);

node_t* tree_copy(node_t* root);
void tree_derive(node_t* root);

double const tree_eval(node_t* root, vardic_t* vardic);
int const tree_is_const(node_t* root);

void tree_free(node_t* root);

#endif
