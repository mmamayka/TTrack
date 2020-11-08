#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "text.h"

char const* const string_errstr(string_err_t const errc) {
	if(errc >= STRING_NERRORS || errc < 0) {
		return NULL;
	}

	static char const* const _TABLE[STRING_NERRORS] = {
		"ok",
		"invalid pointer to the first element",
		"invalid pointer to the last element",
		"invalid state (first > last)",
		"string is null"
	};
	return _TABLE[errc];
}

string_t const string_init(char* const begin, char* const end) {
	assert(begin != NULL);
	assert(end != NULL);
	assert(end >= begin);

	string_t string = { begin, end };
	return string;
}
string_t const string_make(char* const cstr) {
	assert(cstr != NULL);
	
	string_t string = { cstr, cstr + strlen(cstr) };
	return string;
}

size_t const string_length(string_t const* const string) {
	string_assert(string);
	return string->last - string->first;
}

void string_chomp(string_t* const string) {
	string_assert(string);

	while(string->first < string->last && isspace(*string->first)) {
		++string->first;
	}
	while(string->last > string->first && isspace(*(string->last - 1))) {
		--string->last;
	}

	string_assert(string);
}

size_t const string_tok(string_t const* const string, string_t* const toks, 
						size_t const max_toks, char const sep)
{
	string_assert(string);
	assert(toks != NULL);

	size_t ntoks = 0;
	char const* rover = string->first;
	char const* ch = NULL;

	do {
		ch = memchr(rover, sep, string->last - rover);
		if(ch != NULL) {
			toks[ntoks].first = rover;
			toks[ntoks].last = ch;
			rover = ch + 1;
		}
		else {
			toks[ntoks].first = rover;
			toks[ntoks].last = string->last;
		}
		string_chomp(&toks[ntoks]);
	} while(++ntoks < max_toks && ch != NULL);

	return ntoks;
}

int const string_ceq(string_t const* const string, char const* const cstring) {
	string_assert(string);
	assert(cstring != NULL);

	size_t slen = string_length(string);
	size_t cslen = strlen(cstring);
	if(slen != cslen)
		return 0;

	return memcmp(string->first, cstring, slen) == 0;
}

string_err_t const string_check(string_t const* const string) {
	if(string == NULL) { return STRING_ERR_NULL; }
	if(string->first == NULL) { return STRING_ERR_FIRST_PTR; }
	if(string->last == NULL) { return STRING_ERR_LAST_PTR; }
	if(string->last < string->first) { return STRING_ERR_STATE; }
	return STRING_ERR_OK;
}
void string__dump(string_t const* const string, FILE* const stream, 
				  char const* const funcname, char const* const filename,
				  size_t const nline)
{
	assert(stream != NULL);
	assert(ferror(stream) == 0);
	assert(funcname != NULL);
	assert(filename != NULL);

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
}

void string__assert(string_t const* const string, char const* const funcname,
					char const* const filename, size_t const nline)
{
	assert(funcname != NULL);
	assert(filename != NULL);

	if(string_check(string) != STRING_ERR_OK) {
		string__dump(string, stderr, funcname, filename, nline);
		assert(!"invalid string");
	}
}

size_t const fsize(FILE* const file) 
{
	assert(file != NULL);
	assert(ferror(file) == 0);

	long int isize = 0;
	size_t size = FSIZE_ERR;

	if(!fseek(file, 0, SEEK_END) && (isize = ftell(file)) != -1L) {
		size = (size_t)isize;
	}

	rewind(file);

	return (size_t)size;
}

size_t const count_lines(char const * const text, size_t const size, char const sep)
{
	assert(text != NULL);

	size_t nlines = 0;
	for(char const* ch = text; ch < text + size; ++ch) {
		if(*ch == sep)
			++nlines;
	}
	
	return nlines + 1;
}

size_t const split_text(char const* const text, size_t const size, char const sep, 
						string_t* const lines) 
{
	assert(text != NULL);
	assert(lines != NULL);

	// test: empty text

	string_t* curline = lines;

	char const* rover = text;
	for(char const* ch = text; ch < text + size; ++ch) {
		if(*ch == sep) {
			curline->first = rover;
			curline->last  = ch;
			++curline;

			rover = ch + 1;
		}
	}

	curline->first = rover;
	curline->last = text + size;

	return curline - lines + 1;
}

char* const read_text(FILE* const file, size_t* const psize, RT_err_t* const errc)
{
	assert(file != NULL);
	assert(ferror(file) == 0);
	assert(psize != NULL); 

	size_t size = fsize(file);
	if(size == FSIZE_ERR) {
		if(errc != NULL) { *errc = RT_IO; }
		return NULL;
	}

	char* text = (char*)calloc(size + 1, sizeof(char));
	if(text == NULL) {
		if(errc != NULL) { *errc = RT_MEMORY; }
		return NULL;
	}

	if(fread(text, sizeof(char), size, file) != size) {
		free(text);
		if(errc != NULL) { *errc = RT_IO; }
		return NULL;
	}

	*psize = size;
	if(errc != NULL) { *errc = RT_OK; }
	return text;
}

int const write_lines(FILE* const file, string_t const * const lines, 
					   size_t const nlines) 
{
	assert(file != NULL);
	assert(ferror(file) == 0);
	assert(lines != NULL);

	for(size_t i = 0; i < nlines; ++i) {
		size_t size = lines[i].last - lines[i].first;
		if(fwrite(lines[i].first, sizeof(char), size, file) != size)
			return 0;

		if(fputc((int)'\n', file) == EOF)
			return 0;
	}

	return 1;
}

string_t* const get_text_lines(char const* const text, size_t const size, char const sep, 
							   size_t* const pnlines)
{
	assert(text != NULL);
	assert(pnlines != NULL);

	size_t const nlines = count_lines(text, size, sep);

	string_t* lines = (string_t*)calloc(nlines, sizeof(string_t));
	if(lines == NULL) {
		return NULL;
	}

	size_t nlines_check = split_text(text, size, sep, lines);
	assert(nlines == nlines_check);

	*pnlines = nlines;
	return lines;
}

char* const read_text2(char const* const fname, size_t* const psize, 
						  RF_err_t* const errc)
{
	assert(fname != NULL);
	assert(psize != NULL);

	FILE* const ifile = fopen(fname, "rb");
	if(ifile == NULL) {
		*errc = RF_NOTFOUND;
		return NULL;
	}

	size_t size = 0;

	RT_err_t rt_errc;
	char* text = read_text(ifile, &size, &rt_errc);

	fclose(ifile);

	if(text == NULL) {
		if(errc != NULL ) {
			switch(rt_errc) {
			case RT_OK:
				assert(0); // imposible situation
				break;

			case RT_IO:
				*errc =  RF_STDIO;
				break;
			case RT_MEMORY:
				*errc = RF_MEMORY;
				break;
			}
		}	
		return NULL;
	}

	*psize = size;
	if(errc != NULL) { *errc = RF_OK; }
	return text;
}

int const write_lines2(char const* const fname, string_t const * const lines,
					size_t const nlines) 
{
	assert(fname != NULL);
	assert(lines != NULL);

	FILE* file = fopen(fname, "wb");
	if(file == NULL)
		return 0;

	int errc = write_lines(file, lines, nlines);

	fclose(file);

	if(errc == 0)
		return 0;

	return 1;
}

void repair_text(char* const text, size_t const size) 
{
	assert(text != NULL);

	for(char* ch = text; ch < text + size; ++ch) {
		if(*ch == '\0')
			*ch = '\n';
	}
}

int const write_text(char const* const filename, char const* const text, size_t const size)
{
	assert(filename != NULL);
	assert(text != NULL);

	FILE* file = fopen(filename, "wb");
	if(file == NULL)
		return 0;

	size_t written = fwrite(text, sizeof(char), size, file);

	fclose(file);

	return written == size;
}

char const * const RF_errstr(RF_err_t const errc) {
	switch(errc) {
		case RF_NOTFOUND:
			return "file not found";
			break;

		case RF_MEMORY:
			return "out of memory";
			break;

		case RF_STDIO:
			return "internal IO error";
			break;

		default:
			return "unknown error";
			break;
		}
}
