#ifndef DEBUG_H
#define DEBUG_H

#include <assert.h>
#include <stdio.h>

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

/* \def STACKTRACE_PRINT
 * \brief Распечатывает трассировку стека вызовов в режиме трассировки и отладки.
 *
 * Если не определен макрос STACKTRACE или наоборот определен NDEBUG не делает ничего.
 */

/* \def STACKTRACE_PUSH
 * \brief Помещает данные текущей функции в стек вызовов.
 *
 * Должна вызываться единожды для каждой функции. Все точки выхода из функции
 * должны быть помечены при помощи RETURN, STACKTRACE_POP или $$.
 *
 * Если не определен макрос STACKTRACE или наоборот определен NDEBUG не делает ничего.
 */

/* \def STACKTRACE_POP
 * \brief Извлекает данные о последней вызванной функции из стека вызовов.
 *
 * Если не определен макрос STACKTRACE или наоборот определен NDEBUG не делает ничего.
 */

/* \def $_
 * \brief То же самое, что и STACK_PUSH.
 */

/* \def $$
 * \brief То же самое, что и STACK_POP.
 */

/* \def RETURN
 * \brief Перегрузка return для извлечения данных из стека вызовов при выходе из
 * 		функции.
 *
 * Если не определен макрос STACKTRACE или наоборот определен NDEBUG полностью
 * эквивалентна return.
 */

#if !defined NDEBUG && defined STACKTRACE
#	define STACKTRACE_PRINT stacktrace_print(stderr);

#	define STACKTRACE_PUSH stacktrace__push(__func__, __FILE__, __LINE__);
#	define STACKTRACE_POP  stacktrace__pop();

#	define $_ STACKTRACE_PUSH
#	define $$ STACKTRACE_POP

#	define RETURN(...)			\
	do {						\
		STACKTRACE_POP			\
		return __VA_ARGS__;		\
	} while(0)

#else /* !defined NDEBUG && defined STACKTRACE */
#	define STACKTRACE_PRINT

#	define STACKTRACE_PUSH
#	define STACKTRACE_POP

#	define $_
#	define $$

#	define RETURN(...) return __VA_ARGS__
#endif /* !defined NDEBUG && defined STACKTRACE */

/* \def ASSERT(expr)
 * \brief Перегрузка стандартного assert, распечатывающая трассировку стека вызовов.
 *
 * Если не определен макрос STACKTRACE или наоборот определен NDEBUG полностью
 * эквивалентна assert.
 */

#ifndef NDEBUG
#	define ASSERT_(expr, func, file, line)												\
	do {																				\
		if(!(expr)) {																	\
			fprintf(stderr, "assertion %s failed at %s (%s %zu)\n", #expr, func, file,	\
					(size_t)line);														\
			STACKTRACE_PRINT															\
			assert(0);																	\
		}																				\
	} while(0)

#	define ASSERT(expr) ASSERT_(expr, __func__, __FILE__, __LINE__)
#else /* NDEBUG */

#	define ASSERT(expr) (void)(expr);
#endif /* NDEBUG */

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
 * 		written is THAT ORDER: %s before %i.
 */
void dbg__message(char const* const file, int line, FILE* const stream, 
		char const* const head_format, char const* const body_format, ...);

/** \def DBG(...)
 * \brief Prints debug message formated with printf rules to stdout stream in debug mode.
 * 		Does nothing in release mode (NDEBUG is defined).
 *
 * 	Format of the message: DEBUG <filename> (<line no>): <the message>.
 */

#ifdef NDEBUG
#	define DBG(...) 
#else

#	define DBG(...) 																\
		dbg__message(__FILE__, __LINE__, stdout, "DEBUG %s (%i): ", __VA_ARGS__)
#endif

/** \def stream_assert(stream)
 * \brief Проверяет поток на неравенство NULL и на отсутствие флагов ошибок ferror().
 *
 * Если опререлен NDEBUG не делает ничего.
 *
 * \param[in] stream поток для проверки.
 */

#define stream_assert(stream) 			\
	do {								\
		assert(stream != NULL);			\
		assert(ferror(stream) == 0);	\
	} while(0)

/** \def DUMP_BUFSIZE
 * \brief Максимально возможное количество напечатанных символов за один вызов
 * 		функции dump.
 *
 * Может быть переопределено при компиляции.
 */

#ifndef DUMP_BUFSIZE
#	define DUMP_BUFSIZE (1024 * 4)
#endif

/** \def DUMP_MAXDEPTH
 * \brief Максимальное количество автоматически сгенерированных функцикй dump.
 * 		отступов.
 *
 * Может быть переопределено при компиляции.
 */

#ifndef DUMP_MAXDEPTH
#	define DUMP_MAXDEPTH 10
#endif

/** \def DUMP_HEX_BLOCK_SIZE
 * \brief Размер блока при печати шестнадцатиричных данных при помощи функции dump_hex.
 *
 * Может быть переопределено при компиляции.
 */

#ifndef DUMP_HEX_BLOCK_SIZE
#	define DUMP_HEX_BLOCK_SIZE 4
#endif

/** \def DUMP_HEX_BLOCKS_IN_LINE
 * \brief Число блоков в строке при печати шестнадцатиричных данных при помощи 
 * 		функции dump_hex.
 *
 * Может быть переопределено при компиляции.
 */

#ifndef DUMP_HEX_BLOCKS_IN_LINE
#	define DUMP_HEX_BLOCKS_IN_LINE 4
#endif

/** \def DUMP_PUSHDEPTH
 * \brief Stores DUMP_DEPTH value in local variable.
 *
 */

#define DUMP_PUSHDEPTH \
	size_t dump_depth  = DUMP_DEPTH;

/** \def DUMP_POPDEPTH
 *  \brief Restores DUMP_DEPTH value stored with DUMP_PUSHDEPTH
 *
 */

#define DUMP_POPDEPTH \
	DUMP_DEPTH = dump_depth;

/** \def DUMP_PUSHTFL
 *  \brief Stores DUMP_TAB_FIRST_LINE value in local variable.
 *
 */

#define DUMP_PUSHTFL \
	size_t dump_tab_first_line = DUMP_TAB_FIRST_LINE;

/** \def DUMP_POPFTL
 *  \brief Restores DUMP_TAB_FIRST_LINE value stored with DUMP_PUSHTFL
 *
 */

#define DUMP_POPTFL \
	DUMP_TAB_FIRST_LINE = dump_tab_first_line;

/// Number of '\t' symbols to be inserted in front of dump text
extern size_t DUMP_DEPTH;

/// Stream dump text will be written info
extern FILE*  DUMP_STREAM;

/// Tab first line of dump text or not
extern int    DUMP_TAB_FIRST_LINE;

void dump(char const* format, ...);
void dump_hex(unsigned char const* data, size_t size);
void dump_raw(char const* data, size_t size);

void stacktrace__push(char const* const funcname, char const* const filename, 
					  size_t const nline);

void stacktrace__pop();

void stacktrace_dump_body();
void stacktrace__dump(char const* funcname, char const* filename, size_t nline);

#define stacktrace_dump() \
	stacktrace__dump(__func__, __FILE__, __LINE__)

void stacktrace_print(FILE* const stream);

#endif

