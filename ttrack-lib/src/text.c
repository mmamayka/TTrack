#include <stdlib.h>
#include <assert.h>
#include "text.h"



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

size_t const count_lines(char const * const text, size_t const size) 
{
	assert(text != NULL);

	size_t nlines = 0;
	for(char const* ch = text; ch < text + size; ++ch) {
		if(*ch == '\n')
			++nlines;
	}
	
	return nlines;
}

size_t const split_text(char* const text, size_t const size, string_t* const lines) 
{
	assert(text != NULL);
	assert(lines != NULL);

	// test: empty text

	string_t* curline = lines;

	char* rover = text;
	for(char* ch = text; ch < text + size; ++ch) {
		if(*ch == '\n') {
			*ch = '\0';

			curline->first = rover;
			curline->last  = ch;
			++curline;

			rover = ch + 1;
		}
	}

	lines->first = rover;
	lines->last = text + size;

	return curline - lines;
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

string_t* const get_text_lines(char* const text, size_t const size, size_t* const pnlines)
{
	assert(text != NULL);
	assert(pnlines != NULL);

	size_t const nlines = count_lines(text, size);

	string_t* lines = (string_t*)calloc(nlines, sizeof(string_t));
	if(lines == NULL) {
		return NULL;
	}

	size_t nlines_check = split_text(text, size, lines);
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
