#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include "dbg.h"

void DebugMessage(char const* const file, int line, FILE* const stream, 
		char const* const head_format, char const* const body_format, ...) 
{
	assert(stream != NULL);
	assert(ferror(stream) == 0);
	assert(head_format != NULL);
	assert(body_format != NULL);
	assert(strstr(head_format, "%s") <= strstr(head_format, "%i"));

	fprintf(stream, head_format, file, line);
	
	va_list args;
	va_start(args, body_format);
	vfprintf(stream, body_format, args);
	va_end(args);
	
	fputc((int)'\n', stream);

	fflush(stream);
}
