#include <ttrack/dbg.h>
#include <ttrack/comp.h>
#include <string.h>
#include <stdlib.h>
#include "binbuf.h"

typedef struct {
	binbuf_err_t err;
	size_t size;
	size_t capacity;
	unsigned char* data;
} binbuf_t;

static binbuf_t binbuf;

static binbuf_err_t binbuf__set_error(binbuf_err_t err)
{$_
	ASSERT(err >= 0 && err < BINBUF_NERRORS);
	binbuf.err = err;
	RETURN(err);
$$
}

char const* binbuf_errstr(binbuf_err_t errc)
{$_
	if(errc < 0 || errc >= BINBUF_NERRORS) {
		RETURN(NULL);
	}

	char const* TABLE[BINBUF_NERRORS] = {
		"ok",
		"out of memory",
		"invalid state (size > capacity)",
		"data pointer is null",
		"error in stdio",
		"invalid error value"
	};
	RETURN(TABLE[errc]);
}

binbuf_err_t const binbuf_init(size_t capacity)
{$_
	ASSERT(capacity != 0);

	binbuf.data = (unsigned char*)calloc(sizeof(unsigned char), capacity);
	if(binbuf.data == NULL) {
		RETURN(binbuf__set_error(BINBUF_ERR_MEM));
	}

	binbuf.size = 0;
	binbuf.capacity = capacity;
	binbuf.err = BINBUF_ERR_OK;
	RETURN(binbuf__set_error(BINBUF_ERR_OK));
}
void binbuf_free() 
{$_
	binbuf_assert();
	free(binbuf.data);
$$
}

binbuf_err_t binbuf_check() 
{$_
	if(binbuf.data == NULL) { RETURN(BINBUF_ERR_DATA); }
	if(binbuf.capacity == 0 || binbuf.size > binbuf.capacity) { RETURN(BINBUF_ERR_STATE); }
	if(binbuf.err < 0 || binbuf.err >= BINBUF_NERRORS) { RETURN(BINBUF_ERR_ERRRANGE); }
	if(binbuf.err != BINBUF_ERR_OK) { RETURN(binbuf.err); }
	RETURN(BINBUF_ERR_OK);
}
void binbuf__dump(char const* funcname, char const* filename, size_t nline) 
{$_
	binbuf_err_t errc = binbuf_check();
	dump("binbuf_t dump from %s (%s %zu), reason %i (%s)\n", funcname, filename,
		 nline, errc, binbuf_errstr(errc));

	dump("{\n");

	dump("\terr = %i (%s)\n", binbuf.err, binbuf_errstr(binbuf.err));
	dump("\tsize = %zu\n", binbuf.size);
	dump("\tcapacity = %zu\n", binbuf.capacity);

	dump("\tdata [%p] = {\n", binbuf.data);

	DUMP_DEPTH += 2;
	dump_hex(binbuf.data, binbuf.capacity);
	DUMP_DEPTH -= 2;

	dump("\n\t}\n}\n");
$$
}

void binbuf__assert(char const* funcname, char const* filename, size_t nline)
{$_
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	if(binbuf_check() != BINBUF_ERR_OK) {
		dump("binbuf_assert failed at %s (%s %zu)\n", funcname, filename, nline);
		binbuf__dump(funcname, filename, nline);
		ASSERT(0);
	}
$$
}

static int const binbuf_resize(size_t new_capacity)
{$_
	ASSERT(new_capacity != 0);
	
	unsigned char* new_data = (unsigned char*)realloc(binbuf.data, new_capacity);
	if(new_data == NULL) {
		RETURN(0);
	}

	binbuf.data = new_data;
	binbuf.capacity = new_capacity;
	RETURN(1);
}

binbuf_err_t const binbuf_write(void* data, size_t size)
{$_
	ASSERT(data != NULL);
	binbuf_assert();

	if(binbuf.capacity - binbuf.size < size) {
		if(!binbuf_resize(next_2power(binbuf.size + size))) {
			RETURN(binbuf__set_error(BINBUF_ERR_MEM));
		}
	}

	memcpy(binbuf.data + binbuf.size, data, size);
	binbuf.size += size;
	RETURN(binbuf__set_error(BINBUF_ERR_OK));
}

size_t const binbuf_pos()
{$_
	binbuf_assert();
	RETURN(binbuf.size);
}

binbuf_err_t const binbuf_flush(FILE* stream)
{$_
	stream_assert(stream);

	size_t written = fwrite(binbuf.data, sizeof(unsigned char), binbuf.size, stream);
	RETURN(binbuf__set_error(written == binbuf.size ? BINBUF_ERR_OK : BINBUF_ERR_STDIO));
}

void binbuf_reset() 
{$_
	binbuf_assert();
	binbuf.size = 0;
$$
}


binbuf_err_t const binbuf_error()
{$_
	RETURN(binbuf.err);
$$
}
int const binbuf_ok()
{$_
	RETURN(binbuf.err == BINBUF_ERR_OK);
$$
}
