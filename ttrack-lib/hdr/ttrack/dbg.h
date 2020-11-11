#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>
#include <stdio.h>

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

#define ASSERT_(expr, func, file, line)													\
	do {																				\
		if(!(expr)) {																	\
			fprintf(stderr, "assertion %s failed at %s (%s %zu)\n", #expr, func, file,	\
					(size_t)line);														\
			stacktrace_print(stderr);													\
			assert(0);																	\
		}																				\
	} while(0)

#define ASSERT(expr) ASSERT_(expr, __func__, __FILE__, __LINE__)

#ifdef STACKTRACE
#define RETURN(...)				\
	do {						\
		$$						\
		return (__VA_ARGS__);	\
	} while(0)
#else
#define RETURN(...) return (__VA_ARGS__)
#endif

/**
 * \brief Prints formated debug message.
 *
 * Firstly, prints head head format (like "ERROR AT %s (%i):"), secondly
 * prints body format with assotiated variadic arguments 
 * (like "SOME SHIT HAPPEN"), thidrly moves carret to the next line.
 *
 * \param [in] stream output stream the formated message to be printed.
 * \param [in] head_format format of the message header. May contain %s and %i 
 * 		sequences. 
 * \param [in] body_format format of the message body 
 * \param [in] ... associated with body_format variadic arguments.
 *
 * \warning Private function for DBG and ERR macro. Don't use it unless you have
 * 		your head up your ass. Otherwise %s and %i sequences in head_format must be 
 * 		written is THAT ORDER: %s before %i. They are hardcoded in printf.
 */
void dbg__message(char const* const file, int line, FILE* const stream, 
		char const* const head_format, char const* const body_format, ...);

/* \def DBG(...)
 * \brief Prints debug message formated with printf rules to stdout stream in debug mode.
 * 		Does nothing in release mode (NDEBUG is defined).
 *
 * 	Format of the message: DEBUG <filename> (<line no>): <the message>.
 */

/* \def ERR(...)
 * \brief Prints error message formated with printf rules to stderr stream.
 *
 * Format of the message in debug mode (NDEBUG isn't defined):
 * 		ERROR AT <filename> (<line no>): <error message>.
 *
 * Format of the message in release mode (NDEBUG is defined):
 * 		ERROR: <error message>.
 */

#ifdef NDEBUG
#	define DBG(...) 
#else

#	define DBG(...) 																\
		dbg__message(__FILE__, __LINE__, stdout, "DEBUG %s (%i): ", __VA_ARGS__)
#endif


#define stream_assert(stream) 			\
	do {								\
		assert(stream != NULL);			\
		assert(ferror(stream) == 0);	\
	} while(0)

#ifndef DUMP_BUFSIZE
#	define DUMP_BUFSIZE (1024 * 4)
#endif

#ifndef DUMP_MAXDEPTH
#	define DUMP_MAXDEPTH 10
#endif

#ifndef DUMP_HEX_BLOCK_SIZE
#	define DUMP_HEX_BLOCK_SIZE 4
#endif

#ifndef DUMP_HEX_BLOCKS_IN_LINE
#	define DUMP_HEX_BLOCKS_IN_LINE 4
#endif


#define DUMP_PUSHDEPTH \
	size_t dump_depth  = DUMP_DEPTH;

#define DUMP_POPDEPTH \
	DUMP_DEPTH = dump_depth;

#define DUMP_PUSHTFL \
	size_t dump_tab_first_line = DUMP_TAB_FIRST_LINE;

#define DUMP_POPTFL \
	DUMP_TAB_FIRST_LINE = dump_tab_first_line;


extern size_t DUMP_DEPTH;
extern FILE*  DUMP_STREAM;
extern int    DUMP_TAB_FIRST_LINE;
void dump(char const* format, ...);
void dump_hex(unsigned char const* data, size_t size);

void stacktrace__push(char const* const funcname, char const* const filename, 
					  size_t const nline);

void stacktrace__pop();

void stacktrace_dump_body();
void stacktrace__dump(char const* funcname, char const* filename, size_t nline);

#define stacktrace_dump() \
	stacktrace__dump(__func__, __FILE__, __LINE__)

#ifdef STACKTRACE
#	define STACKTRACE_PUSH stacktrace__push(__func__, __FILE__, __LINE__);
#	define STACKTRACE_POP  stacktrace__pop();

#	define $_ STACKTRACE_PUSH
#	define $$ STACKTRACE_POP
#else
#	define STACKTRACE_PUSH
#	define STACKTRACE_POP

#	define $_
#	define $$
#endif

#endif

void stacktrace_print(FILE* const stream);
