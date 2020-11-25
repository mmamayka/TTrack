#ifndef TREE_H
#define TREE_H

#include <stddef.h>
#include "node.h"

typedef enum {
	TREE_ERR_OK,
	TREE_ERR_LOOP,
	TREE_ERR_NODE,
	TREE_ERR_NULL,

	TREE_NERRORS
} tree_err_t;

char const* tree_errstr(tree_err_t err);

tree_err_t tree_check(node_t const* root);

void tree__dump(node_t const* root, char const* funcname, char const* filename,
				size_t nline);

#define tree_dump(root) \
	tree__dump(root, __func__, __FILE__, __LINE__)

void tree__assert(node_t const* root, char const* funcname, char const* filename, 
				  size_t nline);

#define tree_assert(root) \
	tree__assert(root, __func__, __FILE__, __LINE__)

void tree_traversal(node_t* root);
void tree_free(node_t* root);

#endif
