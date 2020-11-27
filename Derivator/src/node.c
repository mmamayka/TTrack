#include <ttrack/dbg.h>
#include <stdlib.h>
#include <math.h>
#include "node.h"

ENUMEX_SOURCE(node_type,
	"operator",
	"const value",
	"variable"
);

ENUMEX_SOURCE(node_err,
	"ok",
	"instance pointer is null",
	"invalid node type"
	"invalid node value",
	"invalid child pointer",
	"invalid operator type"
);

ENUMEX_SOURCE(op_type,
	"add",
	"sub",
	"mul",
	"div",
	"pow"
);


node_err_t const node_check(node_t* node)
{$_
	if(node == NULL) { RETURN(NODE_ERR_NULL); }
	if(!node_type_ok(node->type)) { RETURN(NODE_ERR_INVTYPE); }

	switch(node->type) {
	case NODE_TYPE_CONST:
		if(isnan(node->value)) { RETURN(NODE_ERR_INVCONST); }
		break;

	case NODE_TYPE_VAR:
		break;

	case NODE_TYPE_OP:
		if(node->lch == NULL || node->rch == NULL) { RETURN(NODE_ERR_INVCHLD); }
		if(!op_type_ok(node->optype)) { RETURN(NODE_ERR_INVOP); }
		break;
	
	default:
		break;
	}

	RETURN(NODE_ERR_OK);
$$
}

int const node_ok(node_t* node) 
{$_
	RETURN(node_check(node) == NODE_ERR_OK);
$$
}

void node__dump(node_t* node, char const* funcname, char const* filename, size_t nline)
{$_
	node_err_t err = node_check(node);
	dump("node_t [%p] dump from %s (%s %i), reason %i (%s) = {\n",
		 node, funcname, filename, nline, err, node_err_str(err));

	if(node != NULL) {
		dump("\ttype = %i (%s)\n", node->type, node_type_str(node->type));

		switch(node->type) {
		case NODE_TYPE_CONST:
			dump("\tvalue = %lf\n", node->value);
			break;

		case NODE_TYPE_VAR:
			dump("\tvarname = %c\n", node->varname);
			break;

		case NODE_TYPE_OP:
			dump("\toptype = %i (%s)\n", node->optype, op_type_str(node->optype));
			break;

		default:
			break;
		}

		dump("\tlch = %p\n", node->lch);
		dump("\trch = %p\n", node->rch);
	}

	dump("}\n");
$$
}


void node__assert(node_t* node, char const* funcname, char const* filename, size_t nline)
{$_
	if(!node_ok(node)) {
		node__dump(node, funcname, filename, nline);
		ASSERT_(!"invlid node", funcname, filename, nline);
	}
$$
}

