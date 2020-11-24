#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ttrack/dbg.h>
#include "node.h"
#include "io.h"

static int const node__ch_ok(node_t const* lch, node_t const* rch) 
{$_
	RETURN(!((lch == NULL) ^ (rch == NULL)));
$$
}

char const* node_errstr(node_err_t err)
{$_
	ASSERT(err >= 0 && err < NODE_NERRORS);

	char const* TABLE[NODE_NERRORS] = {
		"ok",
		"invalid pointers to child nodes",
		"invalid data",
		"node is null",
		"loop"
	};
	return TABLE[err];
$$
}

node_err_t const node_check(node_t const* node) 
{$_
	if(node == NULL) { RETURN(NODE_ERR_NULL); }
	if(node->data == NULL) { RETURN(NODE_ERR_DATA); }
	if(!node__ch_ok(node->lch, node->rch)) { RETURN(NODE_ERR_CHILDREN); }
	if(node->color != 0) { RETURN(NODE_ERR_LOOP); }
	RETURN(NODE_ERR_OK);
$$
}

void node__dump(node_t const* node, char const* funcname, char const* filename, 
				size_t nline) 
{$_
	node_err_t err = node_check(node);
	dump("node_t [%p] dump from %s (%s %zu), reason %i (%s) {\n",
			funcname, filename, nline, node, err, node_errstr(err));

	if(node != NULL) {
		dump("\tdata [%p] = \'%s\'\n", node->data, node->data);
		dump("\tlch = %p\n", node->lch);
		dump("\trch = %p\n", node->rch);

		dump("\tcolor = %i\n", node->color);
	}
	dump("}\n");
$$
}

void node__assert(node_t const* node, char const* funcname, char const* filename,
				 size_t nline)
{$_
	if(node_check(node) != NODE_ERR_OK) {
		node__dump(node, funcname, filename, nline);
		ASSERT_(!"invalid node", funcname, filename, nline);
	}
$$
}

node_t* const node_make(char const* cdata, node_t* lch, node_t* rch) 
{$_
	ASSERT(cdata != NULL);
	ASSERT(node__ch_ok(lch, rch));

	node_t* node = (node_t*)malloc(sizeof(node_t));
	if(node == NULL) {
		RETURN(NULL);
	}

	size_t dlen = strlen(cdata) + 1;
	char* data = (char*)malloc(sizeof(char) * dlen);
	if(data == NULL) {
		free(node);
		RETURN(NULL);
	}

	memcpy(data, cdata, sizeof(char) * dlen);

	node->data = data;
	node->lch = lch;
	node->rch = rch;

	node->color = 0;

	node_assert(node);

	RETURN(node);
$$
}
node_t* const node_read_answer()
{$_
	size_t const BUF_SIZE = 1024;
	char buf[BUF_SIZE];
	do {
		printf("Who is it? ");
		fflush(stdout);
	} while(!read_word(buf, BUF_SIZE));

	node_t* node = node_make(buf, NULL, NULL);

	node_assert(node);

	RETURN(node);
$$
}
node_t* const node_read_question(node_t* lch, node_t* rch)
{$_
	ASSERT(node__ch_ok(lch, rch));

	size_t const BUF_SIZE = 1024;
	char buf[BUF_SIZE];

	do {
		printf("What question? ");
		fflush(stdout);
	} while(!read_line(buf, BUF_SIZE));

	node_t* node = node_make(buf, lch, rch);

	node_assert(node);

	RETURN(node);
$$
}

#define swap(type, x, y) 	\
	do {					\
		type tmp = x;		\
		x = y;				\
		y = tmp;			\
	} while(0)

void node_swap(node_t* node1, node_t* node2) 
{$_
	node_assert(node1);
	node_assert(node2);

	swap(char*,   node1->data, node2->data);
	swap(node_t*, node1->lch,  node2->lch);
	swap(node_t*, node1->rch,  node2->rch);

	node_assert(node1);
	node_assert(node2);
$$
}
void const node_free(node_t* node) 
{$_
	node_assert(node);
	free(node->data);
	free(node);
$$
}
