#include <stdio.h>
#include <stdlib.h>
#include <ttrack/dbg.h>

#include "tree.h"
#include "treeio.h"
#include "io.h"

int main() 
{$_
	node_t* root = tree_read("database.txt");
	if(root == NULL) {
		fprintf(stderr, "Failed to read database\n");
		RETURN(EXIT_FAILURE);
	}

	while(1) {
		printf("Do you want to play?\n");
		int answer = yes_or_no();

		if(!answer)
			break;

		tree_traversal(root);
	}

	tree_write_file(root, "database.txt");

	tree_free(root);
$$
}
