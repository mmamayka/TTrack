#include <stdarg.h>
#include "log.h"
#include "dbg.h"

#if defined LOGGER_INPUT_ASSERT
#	define LOGGER_ON_INPUT_ASSERT(code) do { code } while(0)
#	define LOGGER_ON_INPUT_ERROR(code)
#elif defined LOGGER_INPUT_ERROR
#	define LOGGER_ON_INPUT_ASSERT(code) 
#	define LOGGER_ON_INPUT_ERROR(code) do { code } while(0)
#else
#	define LOGGER_ON_INPUT_ASSERT(code) 
#	define LOGGER_ON_INPUT_ERROR(code) 
#endif

static char const* const LOGGER__LEVELSTR[LOGGER_NLEVELS] = {
	LOGGER_DEBUG_TITLE,
	LOGGER_INFO_TITLE,
	LOGGER_WARNING_TITLE,
	LOGGER_ERROR_TITLE
};
static char const* const LOGGER__ERRORSTR[LOGGER_NERRORS] = {
	"ok",
	"stdio internal error",
	"invalid argument",
	"log not finished (missed logger_end() call)",
	"invalid log level value",
	"invalid log stream",
	"logger instance pointer is null",
	"reinitiallization probability",
	"invalid ignore flag"
};
static char const* const LOGGER__IGNFLAGSTR[4] = {
	"no logging",
	"logging started",
	"logging ingnored",
	"POISON"
};

static char const* const LOGGER__COLORSTR[LOGGER_NLEVELS] = {
	LOGGER_DEBUG_COLOR,
	LOGGER_INFO_COLOR,
	LOGGER_WARNING_COLOR,
	LOGGER_ERROR_COLOR
};

static int const logger__validate_level(logger_level_t level)
{$_
	RETURN(level >= 0 && level < LOGGER_NLEVELS);
}
static int const logger__validate_error(logger_err_t error)
{$_
	RETURN(error >= 0 && error < LOGGER_NERRORS);
}
static int const logger__validate_ignflag(int ignflag)
{$_
	RETURN(ignflag >= 0 && ignflag < 4);
}

static char const* const logger__colorstr(logger_level_t level)
{$_
	if(logger__validate_level(level)) {
		RETURN(LOGGER__COLORSTR[level]);
	}
	else {
		RETURN("");
	}
}

char const* const logger_levelstr(logger_level_t const level)
{$_
	if(logger__validate_level(level)) {
		RETURN(LOGGER__LEVELSTR[level]);
	}
	else {
		// invalid level value
		RETURN(NULL);
	}
}
char const* const logger_errorstr(logger_err_t const error)
{$_
	if(logger__validate_error(error)) {
		RETURN(LOGGER__ERRORSTR[error]);
	}
	else {
		// invalid error value
		RETURN(NULL);
	}
}
char const* const logger__ignflagstr(int ignflag)
{$_
	if(logger__validate_ignflag(ignflag)) {
		RETURN(LOGGER__IGNFLAGSTR[ignflag]);
	}
	else {
		// invalid ignore flag value
		RETURN(NULL);
	}
}

logger_err_t const logger_init(logger_t* const logger, FILE* const stream, 
							   logger_level_t const min_level, int fclose_flag)
{$_
	LOGGER_ON_INPUT_ASSERT(
		ASSERT(logger != NULL);
		ASSERT(stream != NULL);
		ASSERT(ferror(stream) == 0);
		ASSERT(logger__validate_level(min_level));
#	ifdef LOGGER_REINIT_PROTECTION
		ASSERT(logger->this != logger);
#	endif
	);
	LOGGER_ON_INPUT_ERROR(
		if(logger == NULL) { RETURN(LOGGER_ERR_NULL); }
		if(stream == NULL || ferror(stream) != 0) { RETURN(LOGGER_ERR_INVALID_STREAM); }
		if(!logger__validate_level(min_level)) { RETURN(LOGGER_ERR_INVALID_LEVEL); }
#	ifdef LOGGER_REINIT_PROTECTION
		if(logger->this == logger) { RETURN(LOGGER_ERR_REINIT_PROBABILITY); }
#	endif
	);

	logger->stream = stream;
	logger->min_level = min_level;

	logger->fclose_flag = fclose_flag;
	logger->ignore_flag = LOGGER__NO_LOG;

	RETURN(LOGGER_ERR_OK);
}

logger_err_t const logger_free(logger_t* const logger)
{$_
	logger_assert(logger);

	if(logger->fclose_flag) {
		fclose(logger->stream);
	}

#ifdef LOGGER_REINIT_PROTECTION
	logger->this = NULL;
#endif

	RETURN(LOGGER_ERR_OK);
}

logger_err_t const logger_min_level(logger_t* const logger, logger_level_t level) {
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		ASSERT(logger__validate_level(level));
	);
	LOGGER_ON_INPUT_ERROR(
		if(!logger__validate_level(level)) {
			RETURN(LOGGER_ERR_INVALID_LEVEL);
		}
	);

	logger->min_level = level;
	RETURN(LOGGER_ERR_OK);
}

logger_level_t const logger_get_min_level(logger_t const* const logger)
{$_
	logger_assert(logger);

	RETURN(logger->min_level);
}
char const* const logger_get_min_level_str(logger_t const* const logger)
{$_
	logger_assert(logger);
	
	RETURN(logger_levelstr(logger->min_level));
}

logger_err_t const logger_start(logger_t* const logger, logger_level_t const level)
{$_
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		ASSERT(logger__validate_level(level));
		ASSERT(logger->ignore_flag == LOGGER__NO_LOG);
	);
	LOGGER_ON_INPUT_ERROR(
		if(!logger__validate_level(level)) {
			RETURN(LOGGER_ERR_INVALID_LEVEL);
		}
		if(logger->ignore_flag != LOGGER__NO_LOG) {
			RETURN(LOGGER_ERR_LOG_NOT_FINISHED);
		}
	);

	if(level < logger->min_level) {
		logger->ignore_flag = LOGGER__LOG_IGNORED;
		RETURN(LOGGER_ERR_OK);
	}

	logger->ignore_flag = LOGGER__LOG_STARTED;
	fprintf(logger->stream, "%s[%s][%s]\n", logger__colorstr(level),
			"", logger_levelstr(level));

	RETURN(LOGGER_ERR_OK);
}
logger_err_t const logger_stop(logger_t* const logger)
{$_
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		ASSERT(logger->ignflag != LOGGER__NO_LOG)
	);
	LOGGER_ON_INPUT_ERROR(
		if(logger->ignflag == LOGGER__NO_LOG) {
			RETURN(LOGGER_ERR_LOG_NOT_STARTED);
		}
	);

	logger->ignore_flag = LOGGER__NO_LOG;

	fprintf(logger->stream, LOGGER_NO_COLOR);

	RETURN(LOGGER_ERR_OK);
}
logger_err_t const logger_printf(logger_t* const logger, char const* const format, ...)
{$_
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		ASSERT(format != NULL);
		ASSERT(logger->ignore_flag != LOGGER__NO_LOG);
	);
	LOGGER_ON_INPUT_ERROR(
		if(format == NULL) {
			RETURN(LOGGER_ERR_INVALID_ARGUMENT);
		}
		if(logger->ignore_flag == LOGGER__NO_LOG) {
			RETURN(LOGGER_ERR_LOG_NOT_STARTED);
		}
	);

	if(logger->ignore_flag == LOGGER__LOG_IGNORED)
		RETURN(LOGGER_ERR_OK);

	va_list l;
	va_start(l, format);
	vfprintf(logger->stream, format, l);
	va_end(l);

	RETURN(LOGGER_ERR_OK);
}

logger_err_t const logger_valid(logger_t const* const logger)
{$_
	if(logger == NULL) {
		RETURN(LOGGER_ERR_NULL);
	}
	if(logger->stream == NULL) {
		RETURN(LOGGER_ERR_INVALID_STREAM);
	}
	if(ferror(logger->stream) != 0) {
		RETURN(LOGGER_ERR_STDIO);
	}
	if(!logger__validate_level(logger->min_level)) {
		RETURN(LOGGER_ERR_INVALID_LEVEL);
	}
	if(!logger__validate_ignflag(logger->ignore_flag)) {
		RETURN(LOGGER_ERR_INVALID_IGNORE_FLAG);
	}
	RETURN(LOGGER_ERR_OK);
}

void logger__dump(logger_t const* const logger, FILE* const stream,
				  char const* const funcname, char const* const filename, 
				  size_t const nline) 
{$_
	logger_err_t error = logger_valid(logger);

	fprintf(stream, "logger_t [%p] dump from %s (%s %zu)\n",
			logger, funcname, filename, nline);
	fprintf(stream, "reason %i (%s)\n", error, logger_errorstr(error));

	if(logger != NULL) {
		fprintf(stream, "\tstream [%p] = TODO\n", logger->stream);
		fprintf(stream, "\tmin_level = %i (%s)\n", logger->min_level, 
				logger_levelstr(logger->min_level));
		fprintf(stream, "\tfclose_flag = %i\n", logger->fclose_flag);
		fprintf(stream, "\tignore_flag = %i (%s)\n", logger->ignore_flag,
				logger__ignflagstr(logger->ignore_flag));
	}

	fprintf(stream, "}\n");
$$
}

void logger__assert(logger_t const* const logger, char const* const funcname,
				   char const* const filename, size_t const nline)
{$_
	if(logger_valid(logger) != LOGGER_ERR_OK) {
		logger__dump(logger, stderr, funcname, filename, nline);
		ASSERT(!"invalid logger state");
	}
$$
}
