#include <stdio.h>
#include <ttrack/dbg.h>

#include "tokenizer.h"
#include "vardic.h"
#include "tree.h"

int main() {
	tokenizer_t tokenizer;
	vardic_t vardic;

	if(vardic_init(&vardic) != VARDIC_ERR_OK) {
		fprintf(stderr, "Failed to initialize variables dictionary\n");
	}

	if(tokenizer_init(&tokenizer, "expression.txt") != TOKENIZER_ERR_OK) {
		fprintf(stderr, "Tokenizer internal error\n");
	}

	node_t* root = tree_parse(&tokenizer, &vardic);
	if(root == NULL) {
		fprintf(stderr, "Invalid tree (%s)\n", tree_parse_err_str(TREE_PARSE_ERR));
	}

	node_t* root1 = tree_copy(root);

	tree_free(root);

	tree_todot(root1, stdout);

	vardic_set(&vardic, 'x', 10.0);

	printf("%lf\n", tree_eval(root1, &vardic));

	tree_free(root1);
	tokenizer_free(&tokenizer);
}
