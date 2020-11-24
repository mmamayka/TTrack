#ifndef NODE_H
#define NODE_H

typedef struct node_s {
	char* data;
	struct node_s* lch;
	struct node_s* rch;
#ifndef NDEBUG
	int color; // dfs cycles check
#endif
} node_t;

typedef enum {
	NODE_ERR_OK,
	NODE_ERR_CHILDREN,
	NODE_ERR_DATA,
	NODE_ERR_NULL,
	NODE_ERR_LOOP,

	NODE_NERRORS
} node_err_t;

char const* node_errstr(node_err_t err);

node_err_t const node_check(node_t const* node);
void node__dump(node_t const* node, char const* funcname, char const* filename, 
				size_t nline);

#define node_dump(node) \
	node__dump(node, __func__, __FILE__, __LINE__)

void node__assert(node_t const* node, char const* funcname, char const* filename,
				 size_t nline);

#define node_assert(node) \
	node__assert(node, __func__, __FILE__, __LINE__)

node_t* const node_make(char const* cdata, node_t* lch, node_t* rch);
node_t* const node_read_answer();
node_t* const node_read_question(node_t* lch, node_t* rch);

void node_swap(node_t* node1, node_t* node2);

void const node_free(node_t* node);


#endif
