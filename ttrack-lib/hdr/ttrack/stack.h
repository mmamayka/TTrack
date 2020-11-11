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
#		define STACK__REINIT_PROTECTION_HASH_DEFINED
#	endif
#	ifndef STACK_REINIT_PROTECTION_THIS_PTR
#		define STACK_REINIT_PROTECTION_THIS_PTR
#		define STACK__REINIT_PROTECTION_THIS_DEFINED
#	endif
#endif

#if defined STACK_REINIT_PROTECTION_THIS_PTR || defined STACK_REINIT_PROTECTION_HASH
#define STACK__REINIT
#endif

#ifndef STACK_DATA_PRINTF_SEQ
#	define STACK_DATA_PRINTF_SEQ "%i"
#endif

#ifndef STACK_DATA_T
#	define STACK_DATA_T int
#endif

#ifndef STACK_H
#	define STACK_H

#	define STACK__CONCAT(tok) stack ## _ ## tok
#	define STACK__CONCAT2(tok1, tok2) stack ## _ ## tok1 ## _ ## tok2

#	define STACK__WRAP(x) x

#	define STACK__TYPE_(type, suf) STACK__CONCAT2(type, suf)
#	define STACK__OVERLOAD_(x, y) STACK__CONCAT2(x, y)

#	define STACK__STR(x) #x
#	define STACK__TYPE_STR_(type) STACK__STR(type)

#	include <stddef.h>
#	include <stdio.h>

#	include <assert.h>
#	include <stdlib.h>
#	include <string.h>

#	include "hash.h"
#	include "dbg.h"


// ---------- ERROR CODES ----------- //
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

// ---------- CANARIES ----------- //
#	define STACK_LCANARY_VAL 0xDEADBEEFDEADBABE
#	define STACK_RCANARY_VAL 0x0BAD0C0FFE0CAFFE
#	define STACK_CANARY_PRINTF_SEQ "%zX"

#	define STACK__CANARY_PRINTF_FMT 							\
		"canary = " STACK_CANARY_PRINTF_SEQ 					\
		" (reference value is " STACK_CANARY_PRINTF_SEQ ")\n"

typedef size_t stack_canary_t;

// ---------- HASH ---------- //
#	define STACK_HASH_PRINTF_SEQ "%zX"

#	define STACK__HASH_PRINTF_FMT 								\
		"hash = " STACK_HASH_PRINTF_SEQ 						\
		" (reference value is " STACK_HASH_PRINTF_SEQ ")\n"

typedef size_t stack_hash_t;

// ----------- TEMPLATES MACRO ---------- //

#	define stack_free(type, stack) \
		STACK__OVERLOAD_(STACK__WRAP(type), free) (stack)

#	define stack_push(type, stack, value) \
		STACK__OVERLOAD_(STACK__WRAP(type), push) (stack, value)

#	define stack_pop(type, stack, value) \
		STACK__OVERLOAD_(STACK__WRAP(type), pop) (stack, value)

#	define stack_assert(type, stack) \
		STACK__OVERLOAD_(STACK__WRAP(type), _assert) (stack, __func__, __FILE__, __LINE__)

#	define stack_check(type, stack) \
		STACK__OVERLOAD_(STACK__WRAP(type), check) (stack)

#	define stack_dump(type, stack, stream) \
		STACK__OVERLOAD_(STACK__WRAP(type), _dump) (stack, stream, __func__, __FILE__, \
												   __LINE__)
#	define stack_init(type, stack, capacity) \
		STACK__OVERLOAD_(STACK__WRAP(type), _init_ex) (stack, capacity, #stack, __func__, \
													   __FILE__, __LINE__)
static char const* const stack_errstr(stack_err_t const err);

#endif

#define STACK__TYPE STACK__TYPE_(STACK__WRAP(STACK_DATA_T), t)
#define STACK__TYPE_STR STACK__TYPE_STR_(STACK__WRAP(STACK__TYPE))
#define STACK__STRUCT STACK__TYPE_(STACK__WRAP(STACK_DATA_T), s)
#define STACK__OVERLOAD(name) \
	STACK__OVERLOAD_(STACK__WRAP(STACK_DATA_T), STACK__WRAP(name))

// TODO: TESTS
// TODO: Error instead of assert
// TODO: Abstract stack from hash algorithm
// TODO: Clear data array before free() call on request

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

typedef struct STACK__STRUCT {
#ifdef STACK_CANARY_PROTECTION
	stack_canary_t lcanary;
#endif /* STACK_CANARY_PROTECTION */

#ifdef STACK_REINIT_PROTECTION_THIS_PTR
	struct STACK__STRUCT* this;
#endif /* STACK_REINIT_PROTECTION_THIS_PTR */

#ifdef STACK_INIT_CONTEXT
	char const* varname;
	char const* funcname;
	char const* filename;
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
} STACK__TYPE;


static stack_err_t const STACK__OVERLOAD(_init) (STACK__TYPE* const stack, 
										  size_t const capacity);

static stack_err_t const STACK__OVERLOAD(_init_ex) (STACK__TYPE* const stack, 
								 size_t const capacity,
								 char const* const varname, char const* const funcname,
								 char const* const filename, size_t const nline);

static void STACK__OVERLOAD(free) (STACK__TYPE* const stack);

static stack_err_t const STACK__OVERLOAD(push) (STACK__TYPE* const stack, 
										 STACK_DATA_T const value);
static stack_err_t const STACK__OVERLOAD(pop) (STACK__TYPE* const stack, 
										STACK_DATA_T* const pvalue);


static stack_err_t const STACK__OVERLOAD(check) (STACK__TYPE const* const stack);

static void STACK__OVERLOAD(_dump) (STACK__TYPE const* const stack, FILE* const stream,
							 char const* const varname,
				 			 char const* const filename, size_t const nline);

static void STACK__OVERLOAD(dump_body) (STACK__TYPE const* const stack);


static void STACK__OVERLOAD(_assert)(STACK__TYPE const* const stack,
							  char const* const funcname,
				   			  char const* const filename, size_t const nline);


#include "stack_impl.h"

#undef STACK__HASH_BODY
#undef STACK__HASH_DATA
#undef STACK__HASH

#ifdef STACK__REINIT_PROTECTION_HASH_DEFINED
#	undef STACK__REINIT_PROTECTION_HASH_DEFINED
#	undef STACK_REINIT_PROTECTION_HASH
#endif

#ifdef STACK__REINIT_PROTECTION_THIS_DEFINED
#	undef STACK__REINIT_PROTECTION_THIS_DEFINED
#	undef STACK_REINIT_PROTECTION_THIS_PTR
#endif

#undef STAC__REINIT

#undef STACK__TYPE
#undef STACK__TYPE_STR
#undef STACK__STRUCT
#undef STACK__OVERLOAD


