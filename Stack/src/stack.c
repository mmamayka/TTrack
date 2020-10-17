#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "hash.h"

#define min(x, y) ((x) < (y) ? (x) : (y))

static stack_err_t const stack_resize(stack_t* const stack, size_t const new_capacity);

#ifdef STACK_CANARY_PROTECTION
static size_t const stack_canaries_size();
static stack_canary_t const stack_data_lcanval(stack_t const* const stack);
static stack_canary_t const stack_data_rcanval(stack_t const* const stack);
#endif /* STACK_CANARY_PROTECTION */

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
static stack_hash_t const stack_hash_body(stack_t const* const stack);
static void stack_update_body_hash(stack_t* const stack);
#endif

#ifdef STACK_HASH_PROTECTION
static stack_hash_t const stack_hash_data(stack_t const* const stack);
static void stack_update_data_hash(stack_t* const stack);
#endif

stack_err_t const stack_init_(stack_t* const stack, size_t const capacity) {
	assert(stack != NULL);
#ifdef STACK_REINIT_PROTECTION_THIS_PTR
	assert(stack->this != stack);
#endif

#ifdef STACK_REINIT_PROTECTION_HASH
	assert(stack->body_hash != stack_hash_body(stack));
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

#if defined STACK_HASH_PROTECTION
	stack_update_data_hash(stack);
#endif

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
	stack_update_body_hash(stack);
#endif

	if(capacity != 0) {
		stack_err_t err = stack_resize(stack, capacity);
		if(err == STACK_ERR_MEM)
			return STACK_ERR_MEM;
	}
	else {
		stack->data = NULL;
	}


#ifdef STACK_HASH_PROTECTION
	stack_update_data_hash(stack);
#endif

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
	stack_update_body_hash(stack);
#endif /* STACK_REINIT_PROTECTION_HASH || STACK_HASH_PROTECTION */

	stack_assert(stack);

	return STACK_ERR_OK;
}

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


#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
	stack_update_body_hash(stack);
#endif

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

#if defined STACK_HASH_PROTECTION
	stack_update_data_hash(stack);
#endif

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
	stack_update_body_hash(stack);
#endif

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

#ifdef STACK_HASH_PROTECTION
	stack_update_data_hash(stack);
#endif

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
	stack_update_body_hash(stack);
#endif

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

#ifdef STACK_HASH_PROTECTION
	stack_update_data_hash(stack);
#endif

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
	stack_update_body_hash(stack);
#endif

	stack_assert(stack);

	return STACK_ERR_OK;
}

char const* const stack_errstr(stack_err_t const err) {
	switch(err) {
	case STACK_ERR_OK:
		return "ok";
		break;

	case STACK_ERR_MEM:
		return "out of memory";
		break;

	case STACK_ERR_UNDERFLOW:
		return "underflow";
		break;

	case STACK_ERR_CANARY:
		return "canary is dead =(((";
		break;

	case STACK_ERR_HASH:
		return "invalid hash";
		break;

	case STACK_ERR_REINIT:
		return "reinit probability";
		break;

	case STACK_ERR_NULL:
		return "stack is null";
		break;

	case STACK_ERR_ARGUMENT:
		return "invalid argument";

	case STACK_ERR_STATE:
		return "invalid state";
		break;

	default:
		return "unknown";
		break;
	}
}



#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
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

#ifdef STACK_HASH_PROTECTION
static stack_hash_t const stack_hash_data(stack_t const* const stack) {
	assert(stack != NULL);

	if(stack->data == NULL)
		return 0;

#	ifdef STACK_CANARY_PROTECTION
	return gnu_hash(stack->data - stack_canaries_size(),
			stack->capacity * sizeof(stack_data_t) + stack_canaries_size() * 2);
#	else /* STACK_CANARY_PROTECTION */
	return gnu_hash(stack->data, stack->capacity() * sizeof(stack_data_t));
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
	   stack_data_lcanval(stack) != STACK_LCANARY_VAL ||
	   stack_data_rcanval(stack) != STACK_RCANARY_VAL) {
		return STACK_ERR_CANARY;
	}
#endif

#ifdef STACK_HASH_PROTECTION
	if(stack->body_hash != stack_hash_body(stack) ||
	   stack->data_hash != stack_hash_data(stack)) {
		return STACK_ERR_HASH;
	}
#endif

	return STACK_ERR_OK;
}

void stack__dump(stack_t const* const stack, FILE* stream, char const* const varname,
				 char const* const filename, size_t const nline) {
	assert(stream != NULL);
	assert(ferror(stream) == 0);
	assert(varname != NULL);
	assert(filename != NULL);

	fprintf(stream, "stack_t [%p] dump\n", stack);

	stack_err_t err = stack_check(stack);
	fprintf(stream, "reason: error %i (%s)\n", err, stack_errstr(err));

	fprintf(stream, "called at %s (%s %zu);\n", varname, filename, nline);

	if(stack != NULL) {
#ifdef STACK_INIT_CONTEXT
		fprintf(stream, "name is \"%s\";\n created at %s (%s %zu);\n",
				stack->varname, stack->funcname, stack->filename, stack->nline);
#endif

		fprintf(stream, "{\n");

#ifdef STACK_CANARY_PROTECTION
		fprintf(stream, "\tlcanary = %zu (reference value is %zu)\n", stack->lcanary,
			STACK_LCANARY_VAL);
#endif

#ifdef STACK_REINIT_PROTECTION_THIS_PTR
		fprintf(stream, "\tthis = %p (reference value is %p)\n", stack->this, stack);
#endif

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
		fprintf(stream, "\thash = %zu (reference value is %zu)\n", stack->body_hash,
			stack_hash_body(stack));
#endif

		fprintf(stream, "\tsize = %zu\n", stack->size);
		fprintf(stream, "\tcapacity = %zu\n", stack->capacity);
		fprintf(stream, "\tdata [%p] = {\n", stack->data);

		if(stack->data != NULL) {
#ifdef STACK_CANARY_PROTECTION
			fprintf(stream, "\t\tlcanary = %zx (reference value is %zx)\n",
				stack_data_lcanval(stack), STACK_LCANARY_VAL);
#endif

#ifdef STACK_HASH_PROTECTION
			fprintf(stream, "\t\thash = %zx (reference value is %zx)\n", stack->data_hash,
				stack_hash_data(stack));
#endif

			for(size_t i = 0; i < stack->capacity; ++i) {
				fprintf(stream, "\t\t%c[%zu] = %i\n", i < stack->size ? '*' : ' ', i, 
					*(stack_data_t*)(stack->data + i * sizeof(stack_data_t)));
			}

#ifdef STACK_CANARY_PROTECTION
			fprintf(stream, "\t\trcanary = %zx (reference value is %zx)\n",
				stack_data_rcanval(stack), STACK_RCANARY_VAL);
#endif
		}

		fprintf(stream, "\t}\n");

#ifdef STACK_CANARY_PROTECTION
		fprintf(stream, "\trcanary = %zu (reference value is %zu)\n", stack->rcanary,
			STACK_RCANARY_VAL);
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

#endif
