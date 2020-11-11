#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include "dbg.h"

void dbg__message(char const* const file, int line, FILE* const stream, 
		char const* const head_format, char const* const body_format, ...) 
{$_
	ASSERT(stream != NULL);
	ASSERT(ferror(stream) == 0);
	ASSERT(head_format != NULL);
	ASSERT(body_format != NULL);
	ASSERT(strstr(head_format, "%s") <= strstr(head_format, "%i"));

	fprintf(stream, head_format, file, line);
	
	va_list args;
	va_start(args, body_format);
	vfprintf(stream, body_format, args);
	va_end(args);
	
	fputc((int)'\n', stream);

	fflush(stream);
$$
}


typedef struct frame_s {
	char const* func;
	char const* file;
	size_t line;
	struct frame_s* next;
} frame_t;


size_t DUMP_DEPTH = 0;
FILE*  DUMP_STREAM = NULL;
int    DUMP_TAB_FIRST_LINE = 1;

// TODO: make dump() function nice!
void dump(char const* format, ...) 
{
	if(DUMP_STREAM == NULL)
		DUMP_STREAM = stderr;

	char buf[DUMP_BUFSIZE];
	
	va_list args;
	va_start(args, format);
	int ret = vsnprintf(buf, DUMP_BUFSIZE, format, args);
	va_end(args);

	size_t bufsize = ret < 0 ? DUMP_BUFSIZE : (size_t)ret;

	if(bufsize == DUMP_BUFSIZE) {
		memcpy(buf - 3, "...", 4);
	}

	char nlbuf[DUMP_MAXDEPTH];

	if(DUMP_DEPTH < DUMP_MAXDEPTH) {
		memset(nlbuf, '\t', DUMP_DEPTH);

		if(DUMP_TAB_FIRST_LINE) {
			fwrite(nlbuf, sizeof(char), DUMP_DEPTH, DUMP_STREAM);
		}

		char* pbufrover = buf;
		for(char* bufrover = strchr(pbufrover, '\n'); bufrover != NULL; 
			bufrover = strchr(bufrover + 1, '\n'))
		{
			fwrite(pbufrover, sizeof(char), bufrover - pbufrover + 1, DUMP_STREAM);
			fwrite(nlbuf, sizeof(char), DUMP_DEPTH, DUMP_STREAM);
			pbufrover = bufrover + 1;
		}

		if(pbufrover != buf + bufsize) {
			fputs(pbufrover, DUMP_STREAM);
		}
	}
	else {
		if(DUMP_TAB_FIRST_LINE) {
			memset(nlbuf, '\t', DUMP_MAXDEPTH);
			fwrite(nlbuf, sizeof(char), DUMP_MAXDEPTH, DUMP_STREAM);
		}
		fputs("...\n", DUMP_STREAM);
	}

}

void dump_hex(unsigned char const* data, size_t size) {
	if(data == NULL || size == 0) { return; }

	dump("%.2hhX", data[0]);

	DUMP_PUSHTFL
	DUMP_TAB_FIRST_LINE = 0;

	size_t nblock = 0;
	size_t nval = 1;
	for(size_t i = 1; i < size; ++i) {
		dump("%.2hhX", data[i]);
		++nval;

		if(nval == DUMP_HEX_BLOCK_SIZE) {
			nval = 0;
			++nblock;

			if(nblock == DUMP_HEX_BLOCKS_IN_LINE) {
				nblock = 0;
				dump("\n");
			}
			else {
				dump(" ");
			}
		}
	}
	DUMP_POPTFL
}

static void frame__init(frame_t* const frame, frame_t* const nextframe,
			     char const* const funcname, char const* const filename, 
			     size_t nline)
{
	ASSERT(frame != NULL);
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	frame->func = funcname;
	frame->file = filename;
	frame->line = nline;
	frame->next = nextframe;

}

static void frame__dump_body(frame_t* frame) 
{
	dump("[%p] {\n", frame);
	if(frame != NULL) {
		dump("\tfunc = \"%s\"\n", frame->func);
		dump("\tfile = \"%s\"\n", frame->file);
		dump("\tline = %zu\n", frame->line);
		dump("\tnext = %p\n", frame->next);
	}
	dump("}\n");

}

typedef struct {
	frame_t* last;
	size_t depth;
	size_t ninvframes;
} stacktrace_t;

static stacktrace_t stacktrace = { NULL, 0, 0 };

void stacktrace__push(char const* const funcname, char const* const filename, 
					  size_t const nline)
{
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	frame_t* frame = (frame_t*)malloc(sizeof(frame_t));
	if(frame == NULL) {
		++stacktrace.ninvframes;
	}
	else {
		frame__init(frame, stacktrace.last == NULL ? NULL : stacktrace.last, 
					funcname, filename, nline);
		stacktrace.last = frame;
	}
	++stacktrace.depth;

}

void stacktrace__pop() 
{
	ASSERT(stacktrace.depth != 0);
	if(stacktrace.ninvframes != 0) {
		--stacktrace.ninvframes;
	}
	else {
		frame_t* frame = stacktrace.last;
		stacktrace.last = frame->next;
		free(frame);
	}
	--stacktrace.depth;

}

void stacktrace_dump_body() {

	dump("(global variable at [%p]) {\n", &stacktrace);
	dump("\tdepth = %zu\n", stacktrace.depth);
	dump("\tninvframes = %zu\n", stacktrace.ninvframes);
	dump("\tlast = %p\n", stacktrace.last);
	dump("\tframes =\n\t{\n");

	DUMP_DEPTH += 2;
	frame_t* frame = stacktrace.last;
	while(frame != NULL) {
		frame__dump_body(frame);
		frame = frame->next;
	}
	DUMP_DEPTH -= 2;

	dump("\t}\n}\n");

}

void stacktrace__dump(char const* funcname, char const* filename, size_t nline) 
{
	dump("stacktrace_t dump from %s (%s %zu)\n", funcname, filename, nline);
	stacktrace_dump_body();

}

#define stacktrace_dump() \
	stacktrace__dump(__func__, __FILE__, __LINE__)

void stacktrace_print(FILE* const stream)
{
	(void)stream;
	dump("stacktrace: \n");

	for(size_t i = 0; i < stacktrace.ninvframes; ++i) {
		dump("\t[INVALID FRAME], called from\n");
	}

	frame_t* frame = stacktrace.last;
	while(frame != NULL) {
		if(frame->next != NULL) {
			dump("\t %s (%s %zu), called from\n", frame->func, frame->file, 
					frame->line);
		}
		else {
			dump("\t %s (%s %zu)\n", frame->func, frame->file, 
					frame->line);

		}
		frame = frame->next;
	}

}

