#include <string.h>
#include <ttrack/dbg.h>
#include <stdint.h>
#include "labeldic.h"


typedef struct {
	char const* name;
	size_t addr;
} label_t;

char const* labeldic_errstr(labeldic_err_t errc) {
	ASSERT(errc >= 0 && errc < LABELDIC_NERRORS);

	char const* TABLE[LABELDIC_NERRORS] = {
		"ok",
		"too much labels",
		"invalid state"
	};
	return TABLE[errc];
}

typedef struct {
	labeldic_err_t err;
	size_t size;
	label_t data[LABELDIC_MAX_LABELS];
} labeldic_t;

static labeldic_t labeldic = {};

labeldic_err_t const labeldic__set_error(labeldic_err_t err) 
{$_
	ASSERT(err >= 0 && err < LABELDIC_NERRORS);
	RETURN(labeldic.err = err);
$$
}

static labeldic_err_t const labeldic__push(char const* name, size_t addr)
{$_
	labeldic_assert();
	ASSERT(name != NULL);

	if(labeldic.size == LABELDIC_MAX_LABELS) {
		RETURN(labeldic__set_error(LABELDIC_ERR_OVERFLOW));
	}

	labeldic.data[labeldic.size].name = name;
	labeldic.data[labeldic.size].addr = addr;
	++labeldic.size;

	RETURN(labeldic__set_error(LABELDIC_ERR_OK));
}
static label_t* const labeldic__find(char const* name) 
{$_
	ASSERT(name != NULL);
	labeldic_check();

	for(size_t i = 0; i < labeldic.size; ++i) {
		if(strcmp(labeldic.data[i].name, name) == 0) {
			RETURN(labeldic.data + i);
		}
	}
	RETURN(NULL);
}

labeldic_err_t const labeldic_getaddr(char const* name, size_t* addr)
{$_
	ASSERT(name != NULL);
	ASSERT(addr != NULL);
	labeldic_assert();

	label_t* label = labeldic__find(name);
	if(label == NULL) {
		RETURN(labeldic__push(name, LABELDIC_POISON));
	}
	else {
		*addr = label->addr;
		RETURN(LABELDIC_ERR_OK);
	}
$$
}
labeldic_err_t const labeldic_setaddr(char const* name, size_t addr)
{$_
	ASSERT(name != NULL);
	labeldic_assert();

	label_t* label = labeldic__find(name);
	if(label == NULL) {
		RETURN(labeldic__push(name, addr));
	}
	else {
		label->addr = addr;
		RETURN(LABELDIC_ERR_OK);
	}
}

char const* labeldic_addrname(size_t addr) 
{$_
	labeldic_assert();
	for(label_t* l = labeldic.data; l < labeldic.data + labeldic.size; ++l) {
		if(l->addr == addr) {
			RETURN(l->name);
		}
	}
	RETURN(NULL);
$$
}

size_t const labeldic_size()
{$_
	labeldic_assert();
	return labeldic.size;
}

labeldic_err_t const labeldic_check() {
	if(labeldic.size > LABELDIC_MAX_LABELS) { 
		return labeldic__set_error(LABELDIC_ERR_OVERFLOW); 
	}
	for(size_t i = 0; i < labeldic.size; ++i) {
		if(labeldic.data[i].name == NULL) { 
			return labeldic__set_error(LABELDIC_ERR_STATE); 
		}
	}
	return labeldic__set_error(LABELDIC_ERR_OK);
}

void labeldic__dump(char const* funcname, char const* filename, size_t nline) 
{$_
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	labeldic_err_t errc = labeldic_check();
	dump("labeldic_t dump from %s (%s %zu), reason %i (%s)\n", funcname, filename,
		 nline, errc, labeldic_errstr(errc));
	dump("{\n");

	dump("\tsize = %zu\n", labeldic.size);
	dump("\tdata [%p] = {\n", labeldic.data);

	for(size_t i = 0; i < LABELDIC_MAX_LABELS; ++i) {
		dump("\t\t%c [%zu] [%p] = {\n", i < labeldic.size ? '*' : ' ', i, 
			 &labeldic.data[i]);
		dump("\t\t\tname [%p] = \"%s\"\n", labeldic.data[i].name, labeldic.data[i].name);

		dump("\t\t\taddr = %zu\n", labeldic.data[i].addr);
		dump("\t\t}\n");
	}

	dump("\t}\n}\n");
$$
}

void labeldic__assert(char const* funcname, char const* filename, size_t nline)
{$_
	ASSERT(funcname != NULL);
	ASSERT(filename != NULL);

	if(labeldic_check() != LABELDIC_ERR_OK) {
		dump("labeldic_assert failed at %s (%s %zu)\n", funcname, filename, nline);
		labeldic__dump(funcname, filename, nline);
		ASSERT(0);
	}
$$
}


