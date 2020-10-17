#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdio.h>

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

#if defined STACK_HASH_PROTECTION || defined STACK_REINIT_PROTECTION_HASH
#	define STACK__HASH_BODY
#endif

#if defined STACK_HASH_PROTECTION
#	define STACK__HASH_DATA
#endif

#if defined STACK__HASH_DATA || defined STACK__HASH_BODY
#	define STACK__HASH
#endif

#if defined STACK_REINIT_PROTECTION
#	ifndef STACK_REINIT_PROTECTION_HASH
#		define STACK_REINIT_PROTECTION_HASH
#	endif
#	ifndef STACK_REINIT_PROTECTION_THIS_PTR
#		define STACK_REINIT_PROTECTION_THIS_PTR
#	endif
#endif

#if defined STACK_REINIT_PROTECTION_THIS_PTR || defined STACK_REINIT_PROTECTION_HASH
#define STACK__REINIT
#endif

#define STACK_VARNAME_MAXLEN 30
#define STACK_FUNCNAME_MAXLEN 30
#define STACK_FILENAME_MAXLEN 30

#define STACK_DATA_PRINTF_SEQ "%i"
typedef int stack_data_t;

#ifdef STACK_CANARY_PROTECTION

#	define STACK_LCANARY_VAL 0xDEADBEEFDEADBABE
#	define STACK_RCANARY_VAL 0x0BAD0C0FFE0CAFFE
#	define STACK_CANARY_PRINTF_SEQ "%zX"

#	define STACK__CANARY_PRINTF_FMT 							\
		"canary = " STACK_CANARY_PRINTF_SEQ 					\
		" (reference value is " STACK_CANARY_PRINTF_SEQ ")\n"

typedef size_t stack_canary_t;

#endif /* STACK_CANARY_PROTECTION */

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION

#	define STACK_HASH_PRINTF_SEQ "%zX"

#	define STACK__HASH_PRINTF_FMT 								\
		"hash = " STACK_HASH_PRINTF_SEQ 						\
		" (reference value is " STACK_HASH_PRINTF_SEQ ")\n"

typedef size_t stack_hash_t;
#endif /* STACK_REIINIT_PROTECTION_HASH || STACK_HASH_PROTECTION */

typedef enum {
	STACK_ERR_OK 			= 0,
	STACK_ERR_MEM			= 1,
	STACK_ERR_UNDERFLOW 	= 2,
	STACK_ERR_CANARY		= 3,
	STACK_ERR_HASH			= 4,
	STACK_ERR_REINIT		= 5,
	STACK_ERR_NULL			= 6,
	STACK_ERR_ARGUMENT		= 7,
	STACK_ERR_STATE			= 8,
} stack_err_t;

typedef struct stack_s {
#ifdef STACK_CANARY_PROTECTION
	stack_canary_t lcanary;
#endif /* STACK_CANARY_PROTECTION */

#ifdef STACK_REINIT_PROTECTION_THIS_PTR
	struct stack_s* this;
#endif /* STACK_REINIT_PROTECTION_THIS_PTR */

#ifdef STACK_INIT_CONTEXT
	char varname[STACK_VARNAME_MAXLEN];
	char funcname[STACK_FUNCNAME_MAXLEN];
	char filename[STACK_FILENAME_MAXLEN];
	size_t nline;
#endif /* STACK_INIT_CONTEXT */

	unsigned char* data;
	size_t size;
	size_t capacity;

#if defined STACK_REINIT_PROTECTION_HASH || defined STACK_HASH_PROTECTION
	stack_hash_t body_hash;
#endif /* STACK_REIINIT_PROTECTION_HASH || STACK_HASH_PROTECTION */

#ifdef STACK_HASH_PROTECTION
	stack_hash_t data_hash;
#endif

#ifdef STACK_CANARY_PROTECTION
	stack_canary_t rcanary;
#endif /* STACK_CANARY_PROTECTION */
} stack_t;


stack_err_t const stack_init_(stack_t* const stack, size_t const capacity);

#ifdef STACK_INIT_CONTEXT

stack_err_t const stack_init_ex_(stack_t* const stack, size_t const capacity,
								 char const* const varname, char const* const funcname,
								 char const* const filename, size_t const nline);

#	define stack_init(stack, capacity) \
		stack_init_ex_(stack, capacity, #stack, __func__, __FILE__, __LINE__)
#else
#	define stack_init(stack, capacity) \
		stack_init_(stack, capacity)
#endif /* STACK_INIT_CONTEXT */

void stack_free(stack_t* const stack);

stack_err_t const stack_push(stack_t* const stack, stack_data_t const value);
stack_err_t const stack_pop(stack_t* const stack, stack_data_t* const pvalue);

char const* const stack_errstr(stack_err_t const err);

stack_err_t const stack_check(stack_t const* const stack);

void stack__dump(stack_t const* const stack, FILE* stream, char const* const varname,
				 char const* const filename, size_t const nline);

#define stack_dump(stack, stream) \
	stack__dump(stack, stream, __func__, __FILE__, __LINE__)

void stack__assert(stack_t const* const stack, char const* const funcname,
				   char const* const filename, size_t const nline);

#define stack_assert(stack) \
	stack__assert(stack, __func__, __FILE__, __LINE__)

#endif
