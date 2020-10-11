#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "dbg.h"

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

#ifdef TESTS
#	include "test.h"
#endif /* TESTS */

#include "main.h"

int main() {
#ifdef TESTS
	Test_Comparator();
	Test_RComparator();
#endif

	char const* filename = "Shakespeare.txt";
	
	size_t size = 0;
	RF_err_t rf_err;

	char* const text = ReadFile(filename, &size, &rf_err);
	if(text == 0) {
		switch(rf_err) {
		case RF_NOTFOUND:
			ERR("file \'%s\' not found", filename);
			break;

		default:
			ERR(RFErrStr(rf_err));
			break;
		}
	}

	size_t nlines = 0;
	string_t* lines1 = MakeLines(text, size, &nlines);
	if(lines1 == NULL) {
		free(text);
		ERR("out of memory");
	}

	string_t* lines2 = (string_t*)calloc(sizeof(string_t), nlines);
	if(lines2 == NULL) {
		free(lines1);
		free(text);

		ERR("out of memory");
	}
	memcpy(lines2, lines1, sizeof(string_t) * nlines);

	qsort(lines1, nlines, sizeof(string_t), Comparator);
	qsort(lines2, nlines, sizeof(string_t), RComparator);

	int errc1 = WriteFileLines("Sorted.txt", lines1, nlines);
	int errc2 = WriteFileLines("FTOBSorted.txt", lines2, nlines);

	if(!errc1 || !errc2) {
		free(lines1);
		free(lines2);
		free(text);
		ERR("internal IO error");
	}

	RepairText(text, size);

	if(!WriteFile("Original.rxt", text, size)) {
		free(lines1);
		free(lines2);
		free(text);
		ERR("internal IO error");
	}

	free(lines1);
	free(lines2);
	free(text);

	return(EXIT_SUCCESS);
}

size_t const FileSize(FILE* const file) 
{
	assert(file != NULL);
	assert(ferror(file) == 0);

	long int isize = 0;
	size_t size = FS_ERR;

	if(!fseek(file, 0, SEEK_END) && (isize = ftell(file)) != -1L) {
		size = (size_t)isize;
	}

	rewind(file);

	return (size_t)size;
}

size_t const CountLines(char const * const text, size_t const size) 
{
	assert(text != NULL);

	size_t nlines = 0;
	for(char const* ch = text; ch < text + size; ++ch) {
		if(*ch == '\n')
			++nlines;
	}
	
	return nlines;
}
void Test_CountLines() 
{
	char const* TEST_TEXT = "HELLO\nNEW\nLINE";
	char const* TEST_TEXT_EMPTY_LINE = "HELLO\nNEW\n";

	TEST_IRV(CountLines(TEST_TEXT, strlen(TEST_TEXT)), 3);
	TEST_IRV(CountLines(TEST_TEXT_EMPTY_LINE, strlen(TEST_TEXT_EMPTY_LINE)), 3);
}

size_t const SplitText(char* const text, size_t const size, string_t* const lines) 
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
// TODO:
void Test_SplitText() 
{
	//char const* const TEST_TEXT = "HELLO\nDUDE\nSPLIT\nME\nPLEASE\n\n";
	//size_t NUMLINES = 7;
}

char* const ReadText(FILE* const file, size_t* const psize, RT_err_t* const errc)
{
	assert(file != NULL);
	assert(ferror(file) == 0);
	assert(psize != NULL); 

	size_t size = FileSize(file);
	if(size == FS_ERR) {
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

int Comparator(void const* vs1, void const* vs2) 
{
	string_t* s1 = (string_t*)vs1;
	string_t* s2 = (string_t*)vs2;

	assert(s1 != NULL);
	assert(s2 != NULL);

	char* p1 = s1->first;
	char* p2 = s2->first;

	while(p1 < s1->last && p2 < s2->last) {
		while(!isalpha(*p1) && p1 < s1->last) ++p1;
		while(!isalpha(*p2) && p2 < s2->last) ++p2;

		if		(tolower(*p1) < tolower(*p2)) { return -1; }
		else if (tolower(*p1) > tolower(*p2)) { return 1; }

		++p1;
		++p2;
	}
	
	if 		(s1->last - p1 < s2->last - p2) { return -1; }
	else if	(s1->last - p1 > s2->last - p2) { return 1; }
	else 			  { return 0; }

}
void Test_Comparator()
{
	{
		char* TEST_STRING = "abcde";
		string_t s1 = { TEST_STRING, TEST_STRING + strlen(TEST_STRING) };
		string_t s2 = { TEST_STRING, TEST_STRING + strlen(TEST_STRING) };
		TEST_IRV(Comparator(&s1, &s2), 0);
	}
	{
		char* TEST_STRING1 = "a   b,,,c , , ,de";
		char* TEST_STRING2 = "a,./,b     c.,.,.de";
		string_t s1 = { TEST_STRING1, TEST_STRING1 + strlen(TEST_STRING1) };
		string_t s2 = { TEST_STRING2, TEST_STRING2 + strlen(TEST_STRING2) };
		TEST_IRV(Comparator(&s1, &s2), 0);
	}
	{
		char* TEST_STRING1 = "a   b,,,c , , ,dz";
		char* TEST_STRING2 = "a,./,b     c.,.,.de";
		string_t s1 = { TEST_STRING1, TEST_STRING1 + strlen(TEST_STRING1) };
		string_t s2 = { TEST_STRING2, TEST_STRING2 + strlen(TEST_STRING2) };
		TEST_IRV(Comparator(&s1, &s2), 1);
	}
	{
		char* TEST_STRING1 = "a   b,,,c , , ,da";
		char* TEST_STRING2 = "a,./,b     c.,.,.de";
		string_t s1 = { TEST_STRING1, TEST_STRING1 + strlen(TEST_STRING1) };
		string_t s2 = { TEST_STRING2, TEST_STRING2 + strlen(TEST_STRING2) };
		TEST_IRV(Comparator(&s1, &s2), -1);
	}

}

int RComparator(void const* vs1, void const* vs2) 
{
	string_t* s1 = (string_t*)vs1;
	string_t* s2 = (string_t*)vs2;

	assert(s1 != NULL);
	assert(s2 != NULL);

	char* p1 = s1->last;
	char* p2 = s2->last;

	while(p1 >= s1->first && p2 >= s2->first) {
		while(!isalpha(*p1) && p1 >= s1->first) --p1;
		while(!isalpha(*p2) && p2 >= s2->first) --p2;

		if		(tolower(*p1) < tolower(*p2)) { return -1; }
		else if (tolower(*p1) > tolower(*p2)) { return 1; }

		--p1;
		--p2;
	}
	
	if 		(s1->last - p1 > s2->last - p2) { return -1; }
	else if	(s1->last - p1 < s2->last - p2) { return 1; }
	else 			  { return 0; }

}
void Test_RComparator() 
{
	{
		char* TEST_STRING = "abcde";
		string_t s1 = { TEST_STRING, TEST_STRING + strlen(TEST_STRING) };
		string_t s2 = { TEST_STRING, TEST_STRING + strlen(TEST_STRING) };
		TEST_IRV(Comparator(&s1, &s2), 0);
	}
	{
		char* TEST_STRING1 = "a   b,,,c , , ,de";
		char* TEST_STRING2 = "a,./,b     c.,.,.de";
		string_t s1 = { TEST_STRING1, TEST_STRING1 + strlen(TEST_STRING1) };
		string_t s2 = { TEST_STRING2, TEST_STRING2 + strlen(TEST_STRING2) };
		TEST_IRV(Comparator(&s1, &s2), 0);
	}
	{
		char* TEST_STRING1 = "z   b,,,c , , ,de";
		char* TEST_STRING2 = "a,./,b     c.,.,.de";
		string_t s1 = { TEST_STRING1, TEST_STRING1 + strlen(TEST_STRING1) };
		string_t s2 = { TEST_STRING2, TEST_STRING2 + strlen(TEST_STRING2) };
		TEST_IRV(Comparator(&s1, &s2), 1);
	}
	{
		char* TEST_STRING1 = "a   b,,,c , , ,da";
		char* TEST_STRING2 = "e,./,b     c.,.,.da";
		string_t s1 = { TEST_STRING1, TEST_STRING1 + strlen(TEST_STRING1) };
		string_t s2 = { TEST_STRING2, TEST_STRING2 + strlen(TEST_STRING2) };
		TEST_IRV(Comparator(&s1, &s2), -1);
	}
}

int const WriteLines(FILE* const file, string_t const * const lines, 
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

string_t* const MakeLines(char* const text, size_t const size, size_t* const pnlines)
{
	assert(text != NULL);
	assert(pnlines != NULL);

	size_t const nlines = CountLines(text, size);

	string_t* lines = (string_t*)calloc(nlines, sizeof(string_t));
	if(lines == NULL) {
		return NULL;
	}

	size_t nlines_check = SplitText(text, size, lines);
	assert(nlines == nlines_check);

	*pnlines = nlines;
	return lines;
}

char* const ReadFile(char const* const fname, size_t* const psize, RF_err_t* const errc)
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
	char* text = ReadText(ifile, &size, &rt_errc);

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

int const WriteFileLines(char const* const fname, string_t const * const lines,
					size_t const nlines) 
{
	assert(fname != NULL);
	assert(lines != NULL);

	FILE* file = fopen(fname, "wb");
	if(file == NULL)
		return 0;

	int errc = WriteLines(file, lines, nlines);

	fclose(file);

	if(errc == 0)
		return 0;

	return 1;
}

void RepairText(char* const text, size_t const size) 
{
	assert(text != NULL);

	for(char* ch = text; ch < text + size; ++ch) {
		if(*ch == '\0')
			*ch = '\n';
	}
}

int const WriteFile(char const* const filename, char const* const text, size_t const size) 
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

char const * const RFErrStr(RF_err_t const errc) {
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
