#ifndef NODE_H
#define NODE_H

#include "vardic.h"
#include "enumex.h"

ENUMEX_HEADER(node_type,
	NODE_TYPE_OP,
	NODE_TYPE_CONST,
	NODE_TYPE_VAR
);

ENUMEX_HEADER(node_err,
	NODE_ERR_OK,
	NODE_ERR_NULL,
	NODE_ERR_INVTYPE,
	NODE_ERR_INVCONST,
	NODE_ERR_INVCHLD,
	NODE_ERR_INVOP
);

#define node_type_assert(type) \
	node_type__assert(type, __func__, __FILE__, __LINE__)

ENUMEX_HEADER(op_type,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_POW
);

#define op_type_assert(type) \
	op_type__assert(type, __func__, __FILE__, __LINE__)

typedef struct node_s {
	node_type_t type;

	union {
		op_type_t optype;
		double value;
		char varname;
	};

	struct node_s* lch;
	struct node_s* rch;

	int color;
} node_t;

node_err_t const node_check(node_t* node);
int const node_ok(node_t* node);
void node__dump(node_t* node, char const* funcname, char const* filename, size_t nline);
void node__assert(node_t* node, char const* funcname, char const* filename, size_t nline);

#define node_dump(node) \
	node__dump(node, __func__, __FILE__, __LINE__)

#define node_assert(node) \
	node__assert(node, __func__, __FILE__, __LINE__)

#endif
