#ifndef TTRACK_STRV_H
#define TTRACK_STRV_H

typedef enum {
	STRV_ERR_OK = 0,
	STRV_ERR_NULL,
	STRV_ERR_STATE,
	STRV_ERR_PFIRST,
	STRV_ERR_PLAST,

	STRV_NERRORS
} strv_err_t;

char const* strv_errstr(strv_err_t err);

typedef struct {
	char const* pfirst;
	char const* plast;
} strv_t;

strv_t const strv_init(char const* pfirst, char const* plast);
strv_t const strv_make(char const* string);

size_t const strv_len(strv_t const* strv);
int const strv_empty(strv_t const* strv);
char const strv_nth(strv_t const* strv, size_t idx);
char const strv_front(strv_t const* strv);
char const strv_back(strv_t const* strv);

char const* strv_begin(strv_t const* strv);
char const* strv_end(strv_t const* strv);

void strv_chompf(strv_t* strv);
void strv_chompb(strv_t* strv);
void strv_chomp(strv_t* strv);

void strv_copy(strv_t* strv, char* buf);
char* strv_tostr(strv_t* strv);

int const strv_cmp(strv_t const* lstrv, strv_t const* rstrv);
int const strv_ccmp(strv_t const* strv, char const* cstr);

strv_t const strv_ntok(strv_t* strv, char const* delimers);
size_t const strv_tok(strv_t* strv, char const* delimers, strv_t* tokens,
					  size_t max_count);

strv_err_t const strv_check(strv_t const* strv);
int const strv_ok(strv_t const* strv);

void strv_dump_body(strv_t const* strv);
void strv__dump(strv_t const* strv, char const* funcname, char const* filename,
				size_t nline);

#define strv_dump(strv) \
	strv__dump(strv, __func__, __FILE__, __LINE__)

void strv__assert(strv_t const* strv, char const* funcname, char const* filename,
				  size_t nline);

#define strv_assert(strv) \
	strv__assert(strv, __func__, __FILE__, __LINE__)

#endif /* TTRACK_STRING_VIEW_H */
