#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "hash.h"


static stack_err_t const stack_resize(stack_t* const stack, size_t const new_capacity);

#ifdef STACK_CANARY_PROTECTION
static size_t const stack_canaries_size();
static stack_canary_t const stack_data_lcanval(stack_t const* const stack);
static stack_canary_t const stack_data_rcanval(stack_t const* const stack);
static int const stack_check_data_canaries(stack_t const* const stack);
#endif /* STACK_CANARY_PROTECTION */

#ifdef STACK__HASH_BODY
static stack_hash_t const stack_hash_body(stack_t const* const stack);
static void stack_update_body_hash(stack_t* const stack);
#endif /* STACK__HASH_BODY */

#ifdef STACK__HASH_DATA
static stack_hash_t const stack_hash_data(stack_t const* const stack);
static void stack_update_data_hash(stack_t* const stack);
#endif /* STACK__HASH_DATA */

#ifdef STACK__HASH
static void stack_update_hash(stack_t* const stack);
static int const stack_check_hash(stack_t const* const stack);
#endif

#ifdef STACK__REINIT
static int const stack_check_reinit_prob(stack_t const* const stack);
#endif

stack_err_t const stack_init_(stack_t* const stack, size_t const capacity) {
	assert(stack != NULL);

#ifdef STACK__REINIT
	assert(stack_check_reinit_prob(stack));
#endif

#ifdef STACK_CANARY_PROTECTION
	stack->lcanary = STACK_LCANARY_VAL;
	stack->rcanary = STACK_RCANARY_VAL;
#endif /* STACK_CANARY_PROTECTION */

#ifdef STACK_REINIT_PROTECTION_THIS_PTR
	stack->this = stack;
#endif /* STACK_REINIT_PROTECTION_THIS_PTR */

	stack->size = 0;
	stack->capacity = 0;
	stack->data = NULL;

#ifdef STACK__HASH
	stack_update_hash(stack);
#endif /* STACK__HASH */

	if(capacity != 0) {
		stack_err_t err = stack_resize(stack, capacity);
		if(err == STACK_ERR_MEM)
			return STACK_ERR_MEM;
	}
	else {
		stack->data = NULL;
	}

#ifdef STACK__HASH
	stack_update_hash(stack);
#endif /* STACK__HASH */

	stack_assert(stack);

	return STACK_ERR_OK;
}

// TODO: order of copy/init
#ifdef STACK_INIT_CONTEXT
stack_err_t const stack_init_ex_(stack_t* const stack, size_t const capacity,
								 char const* const varname, char const* const funcname,
								 char const* const filename, size_t const nline)
{
	assert(stack != NULL);

	strncpy(stack->varname, varname, STACK_VARNAME_MAXLEN);
	strncpy(stack->funcname, funcname, STACK_FUNCNAME_MAXLEN);
	strncpy(stack->filename, filename, STACK_FILENAME_MAXLEN);
	stack->nline = nline;

	stack_err_t err = stack_init_(stack, capacity);
	if(err != STACK_ERR_OK) {
		return err;
	}

#ifdef STACK__HASH
	stack_update_hash(stack);
#endif /* STACK__HASH */

	stack_assert(stack);

	return STACK_ERR_OK;
}
#endif /* STACK_INIT_CONTEXT */

void stack_free(stack_t* const stack) {
	stack_assert(stack); // known memleak is better than unknown undefined behaviour

	if(stack->data) {
#ifdef STACK_CANARY_PROTECTION
		free(stack->data - stack_canaries_size());
#else
		free(stack->data);
#endif
	}

	memset(stack, 0, sizeof(stack_t));
}

static stack_err_t const stack_resize(stack_t* const stack, size_t const new_capacity) {
	assert(new_capacity >= stack->size);
	stack_assert(stack);

#ifdef STACK_CANARY_PROTECTION
	size_t canary_size = stack_canaries_size();
	size_t realloc_size = new_capacity * sizeof(stack_data_t) + canary_size * 2;
	unsigned char* old_data = stack->data == NULL ? NULL : stack->data - canary_size;
#else  /* STACK_CANARY_PROTECTION */
	size_t realloc_size = new_capacity * sizeof(stack_data_t);
	unsigned char* old_data = stack->data;
#endif /* STAC_CANARY_PROTECTION */
	
	unsigned char* new_data = (unsigned char*)realloc(old_data, realloc_size);

	if(new_data == NULL) {
		return STACK_ERR_MEM;
	}

#ifdef STACK_CANARY_PROTECTION
	*(stack_canary_t*)new_data = STACK_LCANARY_VAL;
	*(stack_canary_t*)(new_data + realloc_size - sizeof(stack_canary_t)) = 
		STACK_RCANARY_VAL;

	stack->data = new_data + canary_size;
#else  /* STACK_CANARY_PROTECTION */
	stack->data = new_data;
#endif /* STACK_CANARY_PROTECTION */

	stack->capacity = new_capacity;

#ifdef STACK__HASH
	stack_update_hash(stack);
#endif /* STACK__HASH */

	stack_assert(stack);

	return STACK_ERR_OK;
}

stack_err_t const stack_push(stack_t* const stack, stack_data_t const value) {
	stack_assert(stack);

	if(stack->size == stack->capacity) {
		stack_err_t err = stack_resize(stack, stack->capacity * 2);
		switch(err) {
		case STACK_ERR_MEM:
			return STACK_ERR_MEM;
			break;

		default:
			break;
		}
	}

	*(stack_data_t*)(stack->data + stack->size * sizeof(stack_data_t)) = value;
	++stack->size;

#ifdef STACK__HASH
	stack_update_hash(stack);
#endif /* STACK__HASH */

	stack_assert(stack);

	return STACK_ERR_OK;
}

stack_err_t const stack_pop(stack_t* const stack, stack_data_t* const pvalue) {
	assert(pvalue != NULL);
	stack_assert(stack);

	if(stack->size == 0) {
		return STACK_ERR_UNDERFLOW;
	}

	--stack->size;
	*pvalue = *(stack_data_t*)(stack->data + stack->size * sizeof(stack_data_t));

#ifdef STACK__HASH
	stack_update_hash(stack);
#endif /* STACK__HASH */

	stack_assert(stack);

	return STACK_ERR_OK;
}

char const* const stack_errstr(stack_err_t const err) {
	static char const* const TABLE[] = {
		"ok",
		"out of memory",
		"underflow",
		"canary is dead =(((",
		"invalid hash",
		"reinit probability",
		"stack is null",
		"invalid argument",
		"invalid state"
	};

	if(err >= 0 && err < sizeof(TABLE) / sizeof(stack_err_t)) {
		return TABLE[err];
	}
	return "unknown";
}



#ifdef STACK__HASH_BODY
static stack_hash_t const stack_hash_body(stack_t const* const stack) {
	assert(stack != NULL);

	stack_t * const s = (stack_t* const)stack;

	stack_hash_t h0 = s->body_hash;
	s->body_hash = 0;

	stack_hash_t h = gnu_hash((char const*)stack, sizeof(stack_t));

	s->body_hash = h0;

	return h;
}

static void stack_update_body_hash(stack_t* const stack) {
	assert(stack != NULL);
	stack->body_hash = stack_hash_body(stack);
}
#endif

#ifdef STACK__HASH_DATA
static stack_hash_t const stack_hash_data(stack_t const* const stack) {
	assert(stack != NULL);

	if(stack->data == NULL)
		return 0;

#	ifdef STACK_CANARY_PROTECTION
	return gnu_hash(stack->data - stack_canaries_size(),
			stack->capacity * sizeof(stack_data_t) + stack_canaries_size() * 2);
#	else /* STACK_CANARY_PROTECTION */
	return gnu_hash(stack->data, stack->capacity * sizeof(stack_data_t));
#	endif /* STACK_CANARY_PROTECTION */
}

static void stack_update_data_hash(stack_t* const stack) {
	assert(stack != NULL);
	stack->data_hash = stack_hash_data(stack);
}
#endif

stack_err_t const stack_check(stack_t const* const stack) {
	if(stack == NULL)
		return STACK_ERR_NULL;

	if((stack->data == NULL && stack->capacity != 0) ||
	   (stack->data != NULL && stack->capacity == 0)	||
	   (stack->size > stack->capacity)) {
		return STACK_ERR_STATE;
	}

#ifdef STACK_CANARY_PROTECTION
	if(stack->lcanary != STACK_LCANARY_VAL || stack->rcanary != STACK_RCANARY_VAL ||
	   !stack_check_data_canaries(stack)) {
		return STACK_ERR_CANARY;
	}
#endif

#ifdef STACK__HASH
	if(!stack_check_hash(stack)) {
		return STACK_ERR_HASH;
	}
#endif

	return STACK_ERR_OK;
}

void stack__dump_value(stack_data_t const* const value, FILE* const stream) {
	assert(value != NULL);
	assert(stream != NULL);
	assert(ferror(stream) == 0);

#if defined STACK_DATA_PRINTF_SEQ
	fprintf(stream, STACK_DATA_PRINTF_SEQ, *value);
#elif defined STACK_DATA_PRINTF_FUNC
	STACK_DATA_PRINTF_FUNC(value, stream);
#else
#	error "stack data value print method is undefined"
#endif
}

void stack__dump(stack_t const* const stack, FILE* stream, char const* const varname,
				 char const* const filename, size_t const nline) {
	assert(stream != NULL);
	assert(ferror(stream) == 0);
	assert(varname != NULL);
	assert(filename != NULL);

	stack_err_t err = stack_check(stack);
	fprintf(stream, "stack_t [%p] dump, ", stack);
	fprintf(stream, "reason: error %i (%s), ", err, stack_errstr(err));
	fprintf(stream, "called at %s (%s %zu);\n", varname, filename, nline);

	if(stack != NULL) {
#ifdef STACK_INIT_CONTEXT
		fprintf(stream, "name is \"%s\" created at %s (%s %zu);\n",
				stack->varname, stack->funcname, stack->filename, stack->nline);
#endif

		fprintf(stream, "{\n");

#ifdef STACK_CANARY_PROTECTION
		fprintf(stream, "\t" STACK__CANARY_PRINTF_FMT, stack->lcanary, STACK_LCANARY_VAL);
#endif

#ifdef STACK_REINIT_PROTECTION_THIS_PTR
		fprintf(stream, "\tthis = %p (reference value is %p)\n", stack->this, stack);
#endif

#ifdef STACK_INIT_CONTEXT
		fprintf(stream, "\tvarname = \"%s\"\n", stack->varname);
		fprintf(stream, "\tfuncname = \"%s\"\n", stack->funcname);
		fprintf(stream, "\tnline = %zu\n", stack->nline);
#endif

#ifdef STACK__HASH_BODY
		fprintf(stream, "\t" STACK__HASH_PRINTF_FMT,
			   	stack->body_hash, stack_hash_body(stack));
#endif

		fprintf(stream, "\tsize = %zu\n", stack->size);
		fprintf(stream, "\tcapacity = %zu\n", stack->capacity);
		fprintf(stream, "\tdata [%p] = {\n", stack->data);

		if(stack->data != NULL) {
#ifdef STACK_CANARY_PROTECTION
			fprintf(stream, "\t\t" STACK__CANARY_PRINTF_FMT, 
					stack_data_lcanval(stack), STACK_LCANARY_VAL);
#endif

#ifdef STACK__HASH_DATA
			fprintf(stream, "\t\t" STACK__HASH_PRINTF_FMT,
				   	stack->data_hash, stack_hash_data(stack));
#endif

			for(size_t i = 0; i < stack->capacity; ++i) {
				fprintf(stream, "\t\t%c[%zu] = ", i < stack->size ? '*' : ' ', i);
				stack__dump_value(
					(stack_data_t*)(stack->data + i * sizeof(stack_data_t)), stream);
				fputc((int)'\n', stream);
			}

#ifdef STACK_CANARY_PROTECTION
			fprintf(stream, "\t\t" STACK__CANARY_PRINTF_FMT,
				stack_data_rcanval(stack), STACK_RCANARY_VAL);
#endif
		}

		fprintf(stream, "\t}\n");

#ifdef STACK_CANARY_PROTECTION
		fprintf(stream, "\t" STACK__CANARY_PRINTF_FMT,
				stack->rcanary, STACK_RCANARY_VAL);
#endif
		
	}

	fprintf(stream, "}\n");
}

void stack__assert(stack_t const* const stack, char const* const funcname,
	   			   char const* const filename, size_t const nline) {
	if(stack_check(stack) != STACK_ERR_OK) {
		stack__dump(stack, stderr, funcname, filename, nline);
		assert(!"stack assertion failed!");
	}
}


#ifdef STACK_CANARY_PROTECTION
static size_t const stack_canaries_size() {
	if(sizeof(stack_data_t) >= sizeof(stack_canary_t)) {
		return sizeof(stack_data_t);
	}
	else {
		size_t p = sizeof(stack_canary_t) / sizeof(stack_data_t);
		size_t r = sizeof(stack_canary_t) % sizeof(stack_data_t);

		if(r == 0)
			return sizeof(stack_canary_t);
		else
			return (p + 1) * sizeof(stack_data_t);
	}
}

static stack_canary_t const stack_data_lcanval(stack_t const* const stack) {
	assert(stack != NULL);

	if(stack->data == NULL)
		return STACK_LCANARY_VAL;

	return *(stack_canary_t*)(stack->data - stack_canaries_size());
}

static stack_canary_t const stack_data_rcanval(stack_t const* const stack) {
	assert(stack != NULL);

	if(stack->data == NULL)
		return STACK_RCANARY_VAL;

	return *(stack_canary_t*)(stack->data + stack->capacity * sizeof(stack_data_t) +
		stack_canaries_size() - sizeof(stack_canary_t));
}

static int const stack_check_data_canaries(stack_t const* const stack) {
	assert(stack != NULL);
	
	return stack_data_lcanval(stack) == STACK_LCANARY_VAL &&
		stack_data_rcanval(stack) == STACK_RCANARY_VAL;
}

#endif

#ifdef STACK__HASH
static void stack_update_hash(stack_t* const stack) {
	assert(stack != NULL);

#ifdef STACK__HASH_DATA
	stack_update_data_hash(stack);
#endif /* STACK__HASH_DATA */

#ifdef STACK__HASH_BODY
	stack_update_body_hash(stack);
#endif /* STACK_HASH_BODY */
}

static int const stack_check_hash(stack_t const* const stack) {
	assert(stack != NULL);

	int valid = 1;

#ifdef STACK__HASH_DATA
	valid = valid && (stack_hash_data(stack) == stack->data_hash);
#endif

#ifdef STACK__HASH_BODY
	valid = valid && (stack_hash_body(stack) == stack->body_hash);
#endif

	return valid;
}
#endif

#ifdef STACK__REINIT
static int const stack_check_reinit_prob(stack_t const* const stack) {
	assert(stack != NULL);
	int valid = 1;
#ifdef STACK_REINIT_PROTECTION_THIS_PTR
	valid = valid && (stack->this != stack);
#endif /* STACK_REINIT_PROTECTION_THIS_PTR */

#ifdef STACK_REINIT_PROTECTION_HASH
	valid = valid && (stack->body_hash != stack_hash_body(stack));
#endif /* STACK_REINIT_PROTECTION_HASH */

	return valid;
}
#endif
