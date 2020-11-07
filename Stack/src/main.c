#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "log.h"

#ifdef __GNUC__
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define STACK_DATA_T int
#define STACK_DATA_PRINTF_SEQ "%i"
#include "stack.h"
#undef STACK_DATA_T
#undef STACK_DATA_PRINTF_SEQ

#define STACK_DATA_T double
#define STACK_DATA_PRINTF_SEQ "%lf"
#include "stack.h"
#undef STACK_DATA_T
#undef STACK_DATA_PRINTF_SEQ


int main() {
	logger_t l;
	logger_init(&l, stderr, LOGGER_DEBUG, 0);

	stack_int_t stack;
	stack_init(int, &stack, 1);

	stack_push(int, &stack, 10);
	stack_push(int, &stack, 15);
	stack_push(int, &stack, 20);

	stack_dump(int, &stack, stderr);

	for(int i = 0; i < 4; ++i) {
		int v = 0;
		if(stack_pop(int, &stack, &v) == STACK_ERR_UNDERFLOW) {
			printf("shit happen\n");
		}
		else {
			printf("%i\n", v);
		}
	}

	stack_free(int, &stack);

	stack_init(int, &stack, 10);
	stack_free(int, &stack);

	logger_free(&l);
}
