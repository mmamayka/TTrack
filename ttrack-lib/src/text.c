#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbg.h"
#include "text.h"

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
						strv_t* const lines) 
{$_
	ASSERT(text != NULL);
	ASSERT(lines != NULL);

	// test: empty text

	strv_t* curline = lines;

	char * rover = text;
	for(char * ch = text; ch < text + size; ++ch) {
		if(*ch == sep) {
			*curline = strv_init(rover, ch);
			++curline;

			rover = ch + 1;
		}
	}

	*curline = strv_init(rover, text + size);

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

int const write_lines(FILE* const file, strv_t const * const lines, 
					   size_t const nlines) 
{$_
	ASSERT(file != NULL);
	ASSERT(ferror(file) == 0);
	ASSERT(lines != NULL);

	for(size_t i = 0; i < nlines; ++i) {
		size_t size = strv_len(&lines[i]);
		if(fwrite(strv_begin(&lines[i]), sizeof(char), size, file) != size)
			RETURN(0);

		if(fputc((int)'\n', file) == EOF)
			RETURN(0);
	}

	RETURN(1);
}

strv_t* const get_text_lines(char * const text, size_t const size, char const sep, 
							   size_t* const pnlines)
{$_
	ASSERT(text != NULL);
	ASSERT(pnlines != NULL);

	size_t const nlines = count_lines(text, size, sep);

	strv_t* lines = (strv_t*)calloc(nlines, sizeof(strv_t));
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

int const write_lines2(char const* const fname, strv_t const * const lines,
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
