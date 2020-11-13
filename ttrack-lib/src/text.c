#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbg.h"
#include "text.h"

char const* const string_errstr(string_err_t const errc)
{$_
	if(errc >= STRING_NERRORS || errc < 0) {
		RETURN(NULL);
	}

	static char const* const _TABLE[STRING_NERRORS] = {
		"ok",
		"invalid pointer to the first element",
		"invalid pointer to the last element",
		"invalid state (first > last)",
		"string is null"
	};
	RETURN(_TABLE[errc]);
}

string_t const string_init(char* begin, char* end)
{$_
	ASSERT(begin != NULL);
	ASSERT(end != NULL);
	ASSERT(end >= begin);

	string_t string = { begin, end };
	string.last = '\0';
	RETURN(string);
}
string_t const string_make(char* const cstr)
{$_
	ASSERT(cstr != NULL);
	
	string_t string = { cstr, cstr + strlen(cstr) };
	RETURN(string);
}

size_t const string_length(string_t const* const string)
{$_
	string_assert(string);
	RETURN(string->last - string->first);
}

void string_chomp(string_t* const string)
{$_
	string_assert(string);

	while(string->first < string->last && isspace(*string->first)) {
		++string->first;
	}
	while(string->last > string->first && isspace(*(string->last - 1))) {
		--string->last;
	}
	*string->last = '\0';
	string_assert(string);
$$
}

string_err_t const string_check(string_t const* const string)
{$_
	if(string == NULL) { RETURN(STRING_ERR_NULL); }
	if(string->first == NULL) { RETURN(STRING_ERR_FIRST_PTR); }
	if(string->last == NULL) { RETURN(STRING_ERR_LAST_PTR); }
	if(string->last < string->first) { RETURN(STRING_ERR_STATE); }
	RETURN(STRING_ERR_OK);
}

void string__dump(string_t const* const string, FILE* const stream, 
				  char const* const funcname, char const* const filename,
				  size_t const nline)
{$_
	ASSERT(stream != NULL);
	ASSERT(ferror(stream) == 0);
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	string_err_t errc = string_check(string);
	fprintf(stream, "string_t [%p] dump from %s (%s %zu), reason: %i (%s)\n",
			string, funcname, filename, nline, errc, string_errstr(errc));

	fputs("{\n", stream);
	if(string != NULL) {
		fprintf(stream, "\tfirst = %p\n", string->first);
		fprintf(stream, "\tlast = %p\n\n", string->last);

		fprintf(stream, "\t[length] = %li\n", string->last - string->first);
		fprintf(stream, "\t[data] = ");
		if(string->first != NULL && string->last != NULL && 
			string->first <= string->last) {
			
			fputc('\"', stream);
			fwrite(string->first, sizeof(char), string->last - string->first, stream);
			fputs("\"\n", stream);
		}
		else {
			fprintf(stream, "invalid\n");
		}
	}
	fputs("}\n", stream);
$$
}

void string__assert(string_t const* const string, char const* const funcname,
					char const* const filename, size_t const nline)
{$_
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	if(string_check(string) != STRING_ERR_OK) {
		string__dump(string, stderr, funcname, filename, nline);
		ASSERT(!"invalid string");
	}
$$
}

size_t const fsize(FILE* const file) 
{$_
	ASSERT(file != NULL);
	ASSERT(ferror(file) == 0);

	long int isize = 0;
	size_t size = FSIZE_ERR;

	if(!fseek(file, 0, SEEK_END) && (isize = ftell(file)) != -1L) {
		size = (size_t)isize;
	}

	rewind(file);

	RETURN((size_t)size);
}

size_t const count_lines(char const * const text, size_t const size, char const sep)
{$_
	ASSERT(text != NULL);

	size_t nlines = 0;
	for(char const* ch = text; ch < text + size; ++ch) {
		if(*ch == sep)
			++nlines;
	}
	
	RETURN(nlines + 1);
}

size_t const split_text(char * const text, size_t const size, char const sep, 
						string_t* const lines) 
{$_
	ASSERT(text != NULL);
	ASSERT(lines != NULL);

	// test: empty text

	string_t* curline = lines;

	char * rover = text;
	for(char * ch = text; ch < text + size; ++ch) {
		if(*ch == sep) {
			curline->first = rover;
			curline->last  = ch;
			*curline->last = '\0';
			++curline;

			rover = ch + 1;
		}
	}

	curline->first = rover;
	curline->last = text + size;

	RETURN(curline - lines + 1);
}

char* const read_text(FILE* const file, size_t* const psize, RT_err_t* const errc)
{$_
	ASSERT(file != NULL);
	ASSERT(ferror(file) == 0);
	ASSERT(psize != NULL); 

	size_t size = fsize(file);
	if(size == FSIZE_ERR) {
		if(errc != NULL) { *errc = RT_IO; }
		RETURN(NULL);
	}

	char* text = (char*)calloc(size + 1, sizeof(char));
	if(text == NULL) {
		if(errc != NULL) { *errc = RT_MEMORY; }
		RETURN(NULL);
	}

	if(fread(text, sizeof(char), size, file) != size) {
		free(text);
		if(errc != NULL) { *errc = RT_IO; }
		RETURN(NULL);
	}

	*psize = size;
	if(errc != NULL) { *errc = RT_OK; }
	RETURN(text);
}

int const write_lines(FILE* const file, string_t const * const lines, 
					   size_t const nlines) 
{$_
	ASSERT(file != NULL);
	ASSERT(ferror(file) == 0);
	ASSERT(lines != NULL);

	for(size_t i = 0; i < nlines; ++i) {
		size_t size = lines[i].last - lines[i].first;
		if(fwrite(lines[i].first, sizeof(char), size, file) != size)
			RETURN(0);

		if(fputc((int)'\n', file) == EOF)
			RETURN(0);
	}

	RETURN(1);
}

string_t* const get_text_lines(char * const text, size_t const size, char const sep, 
							   size_t* const pnlines)
{$_
	ASSERT(text != NULL);
	ASSERT(pnlines != NULL);

	size_t const nlines = count_lines(text, size, sep);

	string_t* lines = (string_t*)calloc(nlines, sizeof(string_t));
	if(lines == NULL) {
		RETURN(NULL);
	}

	size_t nlines_check = split_text(text, size, sep, lines);
	ASSERT(nlines == nlines_check);

	*pnlines = nlines;
	RETURN(lines);
}

char* const read_text2(char const* const fname, size_t* const psize, 
						  RF_err_t* const errc)
{$_
	ASSERT(fname != NULL);
	ASSERT(psize != NULL);

	FILE* const ifile = fopen(fname, "rb");
	if(ifile == NULL) {
		*errc = RF_NOTFOUND;
		RETURN(NULL);
	}

	size_t size = 0;

	RT_err_t rt_errc;
	char* text = read_text(ifile, &size, &rt_errc);

	fclose(ifile);

	if(text == NULL) {
		if(errc != NULL ) {
			switch(rt_errc) {
			case RT_OK:
				ASSERT(0); // imposible situation
				break;

			case RT_IO:
				*errc =  RF_STDIO;
				break;
			case RT_MEMORY:
				*errc = RF_MEMORY;
				break;
			}
		}	
		RETURN(NULL);
	}

	*psize = size;
	if(errc != NULL) { *errc = RF_OK; }
	RETURN(text);
}

int const write_lines2(char const* const fname, string_t const * const lines,
					size_t const nlines) 
{$_
	ASSERT(fname != NULL);
	ASSERT(lines != NULL);

	FILE* file = fopen(fname, "wb");
	if(file == NULL)
		RETURN(0);

	int errc = write_lines(file, lines, nlines);

	fclose(file);

	if(errc == 0)
		RETURN(0);

	RETURN(1);
}

void repair_text(char* const text, size_t const size) 
{$_
	ASSERT(text != NULL);

	for(char* ch = text; ch < text + size; ++ch) {
		if(*ch == '\0')
			*ch = '\n';
	}
$$
}

int const write_text(char const* const filename, char const* const text, size_t const size)
{$_
	ASSERT(filename != NULL);
	ASSERT(text != NULL);

	FILE* file = fopen(filename, "wb");
	if(file == NULL)
		RETURN(0);

	size_t written = fwrite(text, sizeof(char), size, file);

	fclose(file);

	RETURN(written == size);
}

char const * const RF_errstr(RF_err_t const errc)
{$_
	switch(errc) {
		case RF_NOTFOUND:
			RETURN("file not found");
			break;

		case RF_MEMORY:
			RETURN("out of memory");
			break;

		case RF_STDIO:
			RETURN("internal IO error");
			break;

		default:
			RETURN("unknown error");
			break;
		}
}
