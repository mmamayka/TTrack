#include <ttrack/dbg.h>
#include <string.h>
#include <limits.h>
#include "reginfo.h"

char const* regmap[REGCNT] = {
	"ax",
	"bx",
	"cx",
	"dx"
};

regid_t const get_regid(char const* regname)
{$_
	ASSERT(regname != NULL);
	for(regid_t i = 0; i < REGCNT; ++i) {
		if(strcmp(regmap[i], regname) == 0) {
			RETURN(i);
		}
	}
	RETURN(UCHAR_MAX);
}
char const* const get_regname(regid_t regid)
{$_
	if(regid >= REGCNT) {
		RETURN(NULL);
	}
	RETURN(regmap[regid]);
}
