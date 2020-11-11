

static stack_err_t const STACK__OVERLOAD(_resize) (STACK__TYPE* const stack, 
												   size_t const new_capacity);

#ifdef STACK_CANARY_PROTECTION
static size_t const STACK__OVERLOAD(_canary_size) ();

static stack_canary_t const STACK__OVERLOAD(_data_lcanval) (
		STACK__TYPE const* const stack);

static stack_canary_t const STACK__OVERLOAD(_data_rcanval) (
		STACK__TYPE const* const stack);

static int const STACK__OVERLOAD(_check_data_canaries) (
		STACK__TYPE const* const stack);
#endif /* STACK_CANARY_PROTECTION */

#ifdef STACK__HASH_BODY
static stack_hash_t const STACK__OVERLOAD(_hash_body) (STACK__TYPE const* const stack);
static void STACK__OVERLOAD(_update_body_hash) (STACK__TYPE* const stack);
#endif /* STACK__HASH_BODY */

#ifdef STACK__HASH_DATA
static stack_hash_t const STACK__OVERLOAD(_hash_data) (STACK__TYPE const* const stack);
static void STACK__OVERLOAD(_update_data_hash) (STACK__TYPE* const stack);
#endif /* STACK__HASH_DATA */

#ifdef STACK__HASH
static void STACK__OVERLOAD(_update_hash) (STACK__TYPE* const stack);
static int const STACK__OVERLOAD(_check_hash) (STACK__TYPE const* const stack);
#endif

#ifdef STACK__REINIT
static int const STACK__OVERLOAD(_check_reinit_prob) (STACK__TYPE const* const stack);
#endif

static stack_err_t const STACK__OVERLOAD(_init) (STACK__TYPE* const stack, 
												 size_t const capacity) 
{$_
	ASSERT(stack != NULL);

#ifdef STACK__REINIT
	ASSERT(STACK__OVERLOAD(_check_reinit_prob) (stack));
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
	STACK__OVERLOAD(_update_hash) (stack);
#endif /* STACK__HASH */

	if(capacity != 0) {
		stack_err_t err = STACK__OVERLOAD(_resize) (stack, capacity);
		if(err == STACK_ERR_MEM) {
			RETURN(STACK_ERR_MEM);
		}
	}
	else {
		stack->data = NULL;
	}

#ifdef STACK__HASH
	STACK__OVERLOAD(_update_hash) (stack);
#endif /* STACK__HASH */

	stack_assert(STACK_DATA_T, stack);

	STACKTRACE_POP

	RETURN(STACK_ERR_OK);
}

// TODO: order of copy/init
static stack_err_t const STACK__OVERLOAD(_init_ex) (STACK__TYPE* const stack, 
								 size_t const capacity,
								 char const* const varname, char const* const funcname,
								 char const* const filename, size_t const nline)
{$_
#ifdef STACK_INIT_CONTEXT
	ASSERT(stack != NULL);
	ASSERT(varname != NULL);
	ASSERT(funcname != NULL);

	stack->varname = varname;
	stack->funcname = funcname;
	stack->filename = filename;
	stack->nline = nline;
#else
	(void)varname;
	(void)funcname;
	(void)filename;
	(void)nline;
#endif

	stack_err_t err = STACK__OVERLOAD(_init) (stack, capacity);
	if(err != STACK_ERR_OK) {
		STACKTRACE_POP
		RETURN(err);
	}

#ifdef STACK__HASH
	STACK__OVERLOAD(_update_hash) (stack);
#endif /* STACK__HASH */

	stack_assert(STACK_DATA_T, stack);

	STACKTRACE_POP

	RETURN(STACK_ERR_OK);
}

static void STACK__OVERLOAD(free) (STACK__TYPE* const stack) {$_
	stack_assert(STACK_DATA_T, stack); // known memleak is better than unknown undefined behaviour

	if(stack->data) {
#ifdef STACK_CANARY_PROTECTION
		free(stack->data - STACK__OVERLOAD(_canary_size) ());
#else
		free(stack->data);
#endif
	}

	memset(stack, 0, sizeof(STACK__TYPE));
$$
}

static stack_err_t const STACK__OVERLOAD(_resize) (STACK__TYPE* const stack, 
												   size_t const new_capacity) 
{$_
	ASSERT(new_capacity >= stack->size);
	stack_assert(STACK_DATA_T, stack);

#ifdef STACK_CANARY_PROTECTION
	size_t canary_size = STACK__OVERLOAD(_canary_size) ();
	size_t realloc_size = new_capacity * sizeof(STACK_DATA_T) + canary_size * 2;
	unsigned char* old_data = stack->data == NULL ? NULL : stack->data - canary_size;
#else  /* STACK_CANARY_PROTECTION */
	size_t realloc_size = new_capacity * sizeof(STACK_DATA_T);
	unsigned char* old_data = stack->data;
#endif /* STAC_CANARY_PROTECTION */
	
	unsigned char* new_data = (unsigned char*)realloc(old_data, realloc_size);

	if(new_data == NULL) {
		RETURN(STACK_ERR_MEM);
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
	STACK__OVERLOAD(_update_hash) (stack);
#endif /* STACK__HASH */

	stack_assert(STACK_DATA_T, stack);

	RETURN(STACK_ERR_OK);
}

static stack_err_t const STACK__OVERLOAD(push) (STACK__TYPE* const stack, 
												STACK_DATA_T const value) 
{$_
	stack_assert(STACK_DATA_T, stack);

	if(stack->size == stack->capacity) {
		stack_err_t err = STACK__OVERLOAD(_resize) (stack, stack->capacity * 2);
		switch(err) {
		case STACK_ERR_MEM:
			RETURN(STACK_ERR_MEM);
			break;

		default:
			break;
		}
	}

	*(STACK_DATA_T*)(stack->data + stack->size * sizeof(STACK_DATA_T)) = value;
	++stack->size;

#ifdef STACK__HASH
	STACK__OVERLOAD(_update_hash) (stack);
#endif /* STACK__HASH */

	stack_assert(STACK_DATA_T, stack);

	RETURN(STACK_ERR_OK);
}

static stack_err_t const STACK__OVERLOAD(pop) (STACK__TYPE* const stack, 
										STACK_DATA_T* const pvalue) 
{$_
	ASSERT(pvalue != NULL);
	stack_assert(STACK_DATA_T, stack);

	if(stack->size == 0) {
		RETURN(STACK_ERR_UNDERFLOW);
	}

	--stack->size;
	*pvalue = *(STACK_DATA_T*)(stack->data + stack->size * sizeof(STACK_DATA_T));

#ifdef STACK__HASH
	STACK__OVERLOAD(_update_hash) (stack);
#endif /* STACK__HASH */

	stack_assert(STACK_DATA_T, stack);

	RETURN(STACK_ERR_OK);
}

#ifndef STACK_IMPL_H
#	define STACK_IMPL_H

static char const* const stack_errstr(stack_err_t const err) {$_
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
		RETURN(TABLE[err]);
	}
	RETURN("unknown");
}

#endif



#ifdef STACK__HASH_BODY
static stack_hash_t const STACK__OVERLOAD(_hash_body) (STACK__TYPE const* const stack)
{$_
	ASSERT(stack != NULL);

	STACK__TYPE * const s = (STACK__TYPE* const)stack;

	stack_hash_t h0 = s->body_hash;
	s->body_hash = 0;

	stack_hash_t h = gnu_hash((char const*)stack, sizeof(STACK__TYPE));

	s->body_hash = h0;

	RETURN(h);
}

static void STACK__OVERLOAD(_update_body_hash) (STACK__TYPE* const stack) {$_
	ASSERT(stack != NULL);
	stack->body_hash = STACK__OVERLOAD(_hash_body) (stack);
$$
}
#endif

#ifdef STACK__HASH_DATA
static stack_hash_t const STACK__OVERLOAD(_hash_data) (STACK__TYPE const* const stack)
{$_
	ASSERT(stack != NULL);

	if(stack->data == NULL)
		RETURN(0);

#	ifdef STACK_CANARY_PROTECTION
	RETURN(gnu_hash(stack->data - STACK__OVERLOAD(_canary_size) (),
			stack->capacity * sizeof(STACK_DATA_T) + 
			STACK__OVERLOAD(_canary_size) () * 2));
#	else /* STACK_CANARY_PROTECTION */
	RETURN(gnu_hash(stack->data, stack->capacity * sizeof(STACK_DATA_T)));
#	endif /* STACK_CANARY_PROTECTION */
}

static void STACK__OVERLOAD(_update_data_hash) (STACK__TYPE* const stack) {$_
	ASSERT(stack != NULL);
	stack->data_hash = STACK__OVERLOAD(_hash_data) (stack);
$$
}
#endif

static stack_err_t const STACK__OVERLOAD(check) (STACK__TYPE const* const stack) {$_
	if(stack == NULL)
		RETURN(STACK_ERR_NULL);

	if((stack->data == NULL && stack->capacity != 0) ||
	   (stack->data != NULL && stack->capacity == 0)	||
	   (stack->size > stack->capacity)) {
		RETURN(STACK_ERR_STATE);
	}

#ifdef STACK_CANARY_PROTECTION
	if(stack->lcanary != STACK_LCANARY_VAL || stack->rcanary != STACK_RCANARY_VAL ||
	   !STACK__OVERLOAD(_check_data_canaries) (stack)) {
		RETURN(STACK_ERR_CANARY);
	}
#endif

#ifdef STACK__HASH
	if(!STACK__OVERLOAD(_check_hash) (stack)) {
		RETURN(STACK_ERR_HASH);
	}
#endif

	RETURN(STACK_ERR_OK);
}

static void STACK__OVERLOAD(_dump_value) (STACK_DATA_T const* const value, 
										  FILE* const stream) 
{$_
	ASSERT(value != NULL);
	ASSERT(stream != NULL);
	ASSERT(ferror(stream) == 0);

#if defined STACK_DATA_PRINTF_SEQ
	fprintf(stream, STACK_DATA_PRINTF_SEQ, *value);
#elif defined STACK_DATA_PRINTF_FUNC
	STACK_DATA_PRINTF_FUNC(value, stream);
#else
#	error "stack data value print method is undefined"
#endif
$$
}

static void STACK__OVERLOAD(_dump) (STACK__TYPE const* const stack, FILE* stream, 
							 char const* const varname,
				 			 char const* const filename, size_t const nline) 
{$_
	ASSERT(stream != NULL);
	ASSERT(ferror(stream) == 0);
	ASSERT(varname != NULL);
	ASSERT(filename != NULL);

	stack_err_t err = STACK__OVERLOAD(check) (stack);
	fprintf(stream, STACK__TYPE_STR " [%p] dump, ", stack);
	fprintf(stream, "reason: error %i (%s), ", err, stack_errstr(err));
	fprintf(stream, "called at %s (%s %zu);\n", varname, filename, nline);

	if(stack != NULL) {
#ifdef STACK_INIT_CONTEXT
		fprintf(stream, "name is \"%s\" created at %s (%s %zu);\n",
				stack->varname, stack->funcname, stack->filename, stack->nline);
#endif

		fprintf(stream, "{\n");

#ifdef STACK_CANARY_PROTECTION
		fprintf(stream, "\t" STACK__CANARY_PRINTF_FMT, stack->lcanary, 
				STACK_LCANARY_VAL);
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
			   	stack->body_hash, STACK__OVERLOAD(_hash_body) (stack));
#endif

		fprintf(stream, "\tsize = %zu\n", stack->size);
		fprintf(stream, "\tcapacity = %zu\n", stack->capacity);
		fprintf(stream, "\tdata [%p] = {\n", stack->data);

		if(stack->data != NULL) {
#ifdef STACK_CANARY_PROTECTION
			fprintf(stream, "\t\t" STACK__CANARY_PRINTF_FMT, 
					STACK__OVERLOAD(_data_lcanval) (stack), STACK_LCANARY_VAL);
#endif

#ifdef STACK__HASH_DATA
			fprintf(stream, "\t\t" STACK__HASH_PRINTF_FMT,
				   	stack->data_hash, STACK__OVERLOAD(_hash_data) (stack));
#endif

			for(size_t i = 0; i < stack->capacity; ++i) {
				fprintf(stream, "\t\t%c[%zu] = ", i < stack->size ? '*' : ' ', i);
				STACK__OVERLOAD(_dump_value) (
					(STACK_DATA_T*)(stack->data + i * sizeof(STACK_DATA_T)), stream);
				fputc((int)'\n', stream);
			}

#ifdef STACK_CANARY_PROTECTION
			fprintf(stream, "\t\t" STACK__CANARY_PRINTF_FMT,
				STACK__OVERLOAD(_data_rcanval) (stack), STACK_RCANARY_VAL);
#endif
		}

		fprintf(stream, "\t}\n");

#ifdef STACK_CANARY_PROTECTION
		fprintf(stream, "\t" STACK__CANARY_PRINTF_FMT,
				stack->rcanary, STACK_RCANARY_VAL);
#endif
		
	}

	fprintf(stream, "}\n");
$$
}

static void STACK__OVERLOAD(_assert) (STACK__TYPE const* const stack, 
									  char const* const funcname,
	   			   					  char const* const filename, size_t const nline) 
{$_
	if(STACK__OVERLOAD(check) (stack) != STACK_ERR_OK) {
		STACK__OVERLOAD(_dump) (stack, stderr, funcname, filename, nline);
		ASSERT(!"stack assertion failed!");
	}
$$
}

#ifdef STACK_CANARY_PROTECTION
static size_t const STACK__OVERLOAD(_canary_size) () {$_
	if(sizeof(STACK_DATA_T) >= sizeof(stack_canary_t)) {
		RETURN(sizeof(STACK_DATA_T));
	}
	else {
		size_t p = sizeof(stack_canary_t) / sizeof(STACK_DATA_T);
		size_t r = sizeof(stack_canary_t) % sizeof(STACK_DATA_T);

		if(r == 0)
			RETURN(sizeof(stack_canary_t));
		else
			RETURN((p + 1) * sizeof(STACK_DATA_T));
	}
}

static stack_canary_t const STACK__OVERLOAD(_data_lcanval) (
		STACK__TYPE const* const stack) 
{$_
	ASSERT(stack != NULL);

	if(stack->data == NULL)
		RETURN(STACK_LCANARY_VAL);

	RETURN(*(stack_canary_t*)(stack->data - STACK__OVERLOAD(_canary_size) ()));
}

static stack_canary_t const STACK__OVERLOAD(_data_rcanval) (
		STACK__TYPE const* const stack) 
{$_
	ASSERT(stack != NULL);

	if(stack->data == NULL)
		RETURN(STACK_RCANARY_VAL);

	RETURN(*(stack_canary_t*)(stack->data + stack->capacity * sizeof(STACK_DATA_T) +
		STACK__OVERLOAD(_canary_size) () - sizeof(stack_canary_t)));
}

static int const STACK__OVERLOAD(_check_data_canaries) (STACK__TYPE const* const stack)
{$_
	ASSERT(stack != NULL);
	
	RETURN(STACK__OVERLOAD(_data_lcanval) (stack) == STACK_LCANARY_VAL &&
		STACK__OVERLOAD(_data_rcanval) (stack) == STACK_RCANARY_VAL);
}

#endif

#ifdef STACK__HASH
static void STACK__OVERLOAD(_update_hash) (STACK__TYPE* const stack) {$_
	ASSERT(stack != NULL);

#ifdef STACK__HASH_DATA
	STACK__OVERLOAD(_update_data_hash) (stack);
#endif /* STACK__HASH_DATA */

#ifdef STACK__HASH_BODY
	STACK__OVERLOAD(_update_body_hash) (stack);
#endif /* STACK_HASH_BODY */
$$
}

static int const STACK__OVERLOAD(_check_hash) (STACK__TYPE const* const stack) {$_
	ASSERT(stack != NULL);

	int valid = 1;

#ifdef STACK__HASH_DATA
	valid = valid && (STACK__OVERLOAD(_hash_data) (stack) == stack->data_hash);
#endif

#ifdef STACK__HASH_BODY
	valid = valid && (STACK__OVERLOAD(_hash_body) (stack) == stack->body_hash);
#endif

	RETURN(valid);
}
#endif

#ifdef STACK__REINIT
static int const STACK__OVERLOAD(_check_reinit_prob) (STACK__TYPE const* const stack) {$_
	ASSERT(stack != NULL);
#ifdef STACK_REINIT_PROTECTION_THIS_PTR
	if (stack->this == stack) {
		RETURN(0);
	}
#endif /* STACK_REINIT_PROTECTION_THIS_PTR */

#ifdef STACK_REINIT_PROTECTION_HASH
	if (stack->body_hash == STACK__OVERLOAD(_hash_body) (stack)) {
		RETURN(0);
	}
#endif /* STACK_REINIT_PROTECTION_HASH */

	RETURN(1);
}
#endif
