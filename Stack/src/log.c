#include <stdarg.h>
#include <assert.h>
#include "log.h"

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

static int const logger__validate_level(logger_level_t level) {
	return level >= 0 && level < LOGGER_NLEVELS;
}
static int const logger__validate_error(logger_err_t error) {
	return error >= 0 && error < LOGGER_NERRORS;
}
static int const logger__validate_ignflag(int ignflag) {
	return ignflag >= 0 && ignflag < 4;
}

static char const* const logger__colorstr(logger_level_t level) {
	if(logger__validate_level(level)) {
		return LOGGER__COLORSTR[level];
	}
	else {
		return "";
	}
}

char const* const logger_levelstr(logger_level_t const level) {
	if(logger__validate_level(level)) {
		return LOGGER__LEVELSTR[level];
	}
	else {
		// invalid level value
		return NULL;
	}
}
char const* const logger_errorstr(logger_err_t const error) {
	if(logger__validate_error(error)) {
		return LOGGER__ERRORSTR[error];
	}
	else {
		// invalid error value
		return NULL;
	}
}
char const* const logger__ignflagstr(int ignflag) {
	if(logger__validate_ignflag(ignflag)) {
		return LOGGER__IGNFLAGSTR[ignflag];
	}
	else {
		// invalid ignore flag value
		return NULL;
	}
}

logger_err_t const logger_init(logger_t* const logger, FILE* const stream, 
							   logger_level_t const min_level, int fclose_flag)
{
	LOGGER_ON_INPUT_ASSERT(
		assert(logger != NULL);
		assert(stream != NULL);
		assert(ferror(stream) == 0);
		assert(logger__validate_level(min_level));
#	ifdef LOGGER_REINIT_PROTECTION
		assert(logger->this != logger);
#	endif
	);
	LOGGER_ON_INPUT_ERROR(
		if(logger == NULL) { return LOGGER_ERR_NULL; }
		if(stream == NULL || ferror(stream) != 0) { return LOGGER_ERR_INVALID_STREAM; }
		if(!logger__validate_level(min_level)) { return LOGGER_ERR_INVALID_LEVEL; }
#	ifdef LOGGER_REINIT_PROTECTION
		if(logger->this == logger) { return LOGGER_ERR_REINIT_PROBABILITY; }
#	endif
	);

	logger->stream = stream;
	logger->min_level = min_level;

	logger->fclose_flag = fclose_flag;
	logger->ignore_flag = LOGGER__NO_LOG;

	return LOGGER_ERR_OK;
}

logger_err_t const logger_free(logger_t* const logger) {
	logger_assert(logger);

	if(logger->fclose_flag) {
		fclose(logger->stream);
	}

#ifdef LOGGER_REINIT_PROTECTION
	logger->this = NULL;
#endif

	return LOGGER_ERR_OK;
}

logger_err_t const logger_min_level(logger_t* const logger, logger_level_t level) {
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		assert(logger__validate_level(level));
	);
	LOGGER_ON_INPUT_ERROR(
		if(!logger__validate_level(level)) {
			return LOGGER_ERR_INVALID_LEVEL;
		}
	);

	logger->min_level = level;
	return LOGGER_ERR_OK;
}

logger_level_t const logger_get_min_level(logger_t const* const logger) {
	logger_assert(logger);

	return logger->min_level;
}
char const* const logger_get_min_level_str(logger_t const* const logger) {
	logger_assert(logger);
	
	return logger_levelstr(logger->min_level);
}

logger_err_t const logger_start(logger_t* const logger, logger_level_t const level) {
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		assert(logger__validate_level(level));
		assert(logger->ignore_flag == LOGGER__NO_LOG);
	);
	LOGGER_ON_INPUT_ERROR(
		if(!logger__validate_level(level)) {
			return LOGGER_ERR_INVALID_LEVEL;
		}
		if(logger->ignore_flag != LOGGER__NO_LOG) {
			return LOGGER_ERR_LOG_NOT_FINISHED;
		}
	);

	if(level < logger->min_level) {
		logger->ignore_flag = LOGGER__LOG_IGNORED;
		return LOGGER_ERR_OK;
	}

	logger->ignore_flag = LOGGER__LOG_STARTED;
	fprintf(logger->stream, "%s[%s][%s]\n", logger__colorstr(level),
			"", logger_levelstr(level));

	return LOGGER_ERR_OK;
}
logger_err_t const logger_stop(logger_t* const logger) {
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		assert(logger->ignflag != LOGGER__NO_LOG)
	);
	LOGGER_ON_INPUT_ERROR(
		if(logger->ignflag == LOGGER__NO_LOG) {
			return LOGGER_ERR_LOG_NOT_STARTED;
		}
	);

	logger->ignore_flag = LOGGER__NO_LOG;

	fprintf(logger->stream, LOGGER_NO_COLOR);

	return LOGGER_ERR_OK;
}
logger_err_t const logger_printf(logger_t* const logger, char const* const format, ...) {
	logger_assert(logger);

	LOGGER_ON_INPUT_ASSERT(
		assert(format != NULL);
		assert(logger->ignore_flag != LOGGER__NO_LOG);
	);
	LOGGER_ON_INPUT_ERROR(
		if(format == NULL) {
			return LOGGER_ERR_INVALID_ARGUMENT;
		}
		if(logger->ignore_flag == LOGGER__NO_LOG) {
			return LOGGER_ERR_LOG_NOT_STARTED;
		}
	);

	if(logger->ignore_flag == LOGGER__LOG_IGNORED)
		return LOGGER_ERR_OK;

	va_list l;
	va_start(l, format);
	vfprintf(logger->stream, format, l);
	va_end(l);

	return LOGGER_ERR_OK;
}

logger_err_t const logger_valid(logger_t const* const logger) {
	if(logger == NULL) {
		return LOGGER_ERR_NULL;
	}
	if(logger->stream == NULL) {
		return LOGGER_ERR_INVALID_STREAM;
	}
	if(ferror(logger->stream) != 0) {
		return LOGGER_ERR_STDIO;
	}
	if(!logger__validate_level(logger->min_level)) {
		return LOGGER_ERR_INVALID_LEVEL;
	}
	if(!logger__validate_ignflag(logger->ignore_flag)) {
		return LOGGER_ERR_INVALID_IGNORE_FLAG;
	}
	return LOGGER_ERR_OK;
}

void logger__dump(logger_t const* const logger, FILE* const stream,
				  char const* const funcname, char const* const filename, 
				  size_t const nline) 
{
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
}

void logger__assert(logger_t const* const logger, char const* const funcname,
				   char const* const filename, size_t const nline)
{
	if(logger_valid(logger) != LOGGER_ERR_OK) {
		logger__dump(logger, stderr, funcname, filename, nline);
		assert(!"invalid logger state");
	}
}
