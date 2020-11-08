#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include <ttrack/dbg.h>
#include <ttrack/text.h>

#ifdef __GNUC__

// I like to return const values from functions, but GCC doesn't.
// So this line will make us up.
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"

#endif /* __GNUC__ */

#ifdef TESTS
#	include <ttrack/test.h>
#endif /* TESTS */

int Comparator(void const* vs1, void const* vs2);
void Test_Comparator();

int RComparator(void const* vs1, void const* vs2);
void Test_RComparator();

int main() {
#ifdef TESTS
	Test_Comparator();
	Test_RComparator();
#endif

	char const* filename = "Shakespeare.txt";
	
	size_t size = 0;
	RF_err_t rf_err;

	char* const text = read_text2(filename, &size, &rf_err);
	if(text == 0) {
		switch(rf_err) {
		case RF_NOTFOUND:
			ERR("file \'%s\' not found", filename);
			break;

		default:
			ERR(RF_errstr(rf_err));
			break;
		}
	}

	size_t nlines = 0;
	string_t* lines1 = get_text_lines(text, size, &nlines);
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

	int errc1 = write_lines2("Sorted.txt", lines1, nlines);
	int errc2 = write_lines2("FTOBSorted.txt", lines2, nlines);

	if(!errc1 || !errc2) {
		free(lines1);
		free(lines2);
		free(text);
		ERR("internal IO error");
	}

	repair_text(text, size);

	if(!write_text("Original.rxt", text, size)) {
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

