#ifndef LOG_H
#define LOG_H

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

// TODO: Disable coloring on non-linux machines

#ifndef LOGGER_DISABLE_COLOR

#	ifndef LOGGER_ERROR_COLOR 
#		define LOGGER_ERROR_COLOR "\033[1;31m"
#	endif

#	ifndef LOGGER_WARNING_COLOR
#		define LOGGER_WARNING_COLOR "\033[1;33m"
#	endif

#	ifndef LOGGER_INFO_COLOR 
#		define LOGGER_INFO_COLOR "\033[1;34m"
#	endif

#	ifndef LOGGER_DEBUG_COLOR
#		define LOGGER_DEBUG_COLOR ""
#	endif

#	ifndef LOGGER_NO_COLOR
#		define LOGGER_NO_COLOR "\033[m;"
#	endif

#endif

// locatisation prototype
#ifndef LOGGER_ERROR_TITLE
#	define LOGGER_ERROR_TITLE "ERROR"
#endif

#ifndef LOGGER_WARNING_TITLE
#	define LOGGER_WARNING_TITLE "WARNING"
#endif

#ifndef LOGGER_INFO_TITLE
#	define LOGGER_INFO_TITLE "INFO"
#endif

#ifndef LOGGER_DEBUG_TITLE
#	define LOGGER_DEBUG_TITLE "DEBUG"
#endif

#define LOGGER__NO_LOG 0
#define LOGGER__LOG_STARTED 1
#define LOGGER__LOG_IGNORED 2

#include <stdio.h>

typedef enum {
	LOGGER_DEBUG 		= 0,
	LOGGER_INFO 		= 1,
	LOGGER_WARNING 	= 2,
	LOGGER_ERROR 		= 3,

	LOGGER_NLEVELS
} logger_level_t;

typedef enum {
	LOGGER_ERR_OK = 0,
	LOGGER_ERR_STDIO,
	LOGGER_ERR_INVALID_ARGUMENT,
	LOGGER_ERR_LOG_NOT_FINISHED,
	LOGGER_ERR_LOG_NOT_STARTED,
	LOGGER_ERR_INVALID_LEVEL,
	LOGGER_ERR_INVALID_STREAM,
	LOGGER_ERR_NULL,
	LOGGER_ERR_REINIT_PROBABILITY,
	LOGGER_ERR_INVALID_IGNORE_FLAG,

	LOGGER_NERRORS
} logger_err_t;

typedef struct logger__s {

#ifdef LOGGER_REINIT_PROTECTION
	struct logger__s this;
#endif

	FILE* stream;

	logger_level_t min_level;

	int fclose_flag;
	int ignore_flag;
} logger_t;

char const* const logger_levelstr(logger_level_t const level);
char const* const logger_errorstr(logger_err_t const error);

logger_err_t const logger_init(logger_t* const logger, FILE* const stream,
							   logger_level_t const min_level, int fclose_flag);
logger_err_t const logger_free(logger_t* const logger);

logger_err_t const logger_min_level(logger_t* const logger, logger_level_t level);
logger_level_t const logger_get_min_level(logger_t const* const logger);
char const* const logger_get_min_level_str(logger_t const* const logger);

logger_err_t const logger_start(logger_t* const logger, logger_level_t const level);
logger_err_t const logger_stop(logger_t* const logger);
logger_err_t const logger_printf(logger_t* const logger, char const* const format, ...);
int const logger_get_ignore_flag(logger_t const* const logger);

logger_err_t const logger_printf0(logger_t* const logger, logger_level_t const level,
								  char const* const format, ...);

logger_err_t const logger_valid(logger_t const* const logger);
void logger__dump(logger_t const* const logger, FILE* const stream,
				  char const* const funcname, char const* const filename, 
				  size_t const nline);

#define logger_dump(logger, stream) \
	logger__dump(logger, stream, __func__, __FILE__, __LINE__)

void logger__assert(logger_t const* const logger, char const* const funcname,
				   char const* const filename, size_t const nline);

#ifdef LOGGER_DEBUG
#	define logger_assert(logger) logger_assert(logger, __func__, __FILE__, __LINE__)
#else
#	define logger_assert(logger)
#endif

#endif
