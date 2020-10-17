#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
#define STACK_CANARY_PROTECTION
#define STACK_REINIT_PROTECTION_THIS_PTR
#define STACK_REINIT_PROTECTION_HASH
#define STACK_HASH_PROTECTION
#define STACK_INIT_CONTEXT
*/

#include "stack.h"

int main() {
	stack_t stack;
	stack_init(&stack, 1);
	
	stack_push(&stack, 10);
	stack_push(&stack, 15);
	stack_push(&stack, 20);

	for(int i = 0; i < 4; ++i) {
		int v = 0;
		if(stack_pop(&stack, &v) == STACK_ERR_UNDERFLOW) {
			printf("shit happen\n");
		}
		else {
			printf("%i\n", v);
		}
	}

	stack_free(&stack);
}
