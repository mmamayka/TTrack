#include <ttrack/dbg.h>
#include "writer.h"
#include "tree.h"

void tree__write_indent(FILE* stream, size_t depth) 
{$_
	for(size_t i = 0; i < depth; ++i) { fputc('\t', stream); }
$$
}

void tree__write(node_t* root, FILE* stream, size_t depth) 
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
