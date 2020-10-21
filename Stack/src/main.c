#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stack.h"

void f(int x) {
	(void)x;
}

int main() {
	stack_t stack;
	stack_init(&stack, 1);
	
	stack_push(&stack, 10);
	stack_push(&stack, 15);
	stack_push(&stack, 20);

	stack_dump(&stack, stdout);

	f(5);

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

	stack_init(&stack, 10);
	stack_free(&stack);
}
