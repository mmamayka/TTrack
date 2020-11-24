#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <ttrack/dbg.h>
#include "io.h"

int const read_word(char buf[], size_t size) 
{$_
	ASSERT(buf != NULL);

	size_t const LINEBUF_SIZE = 32;
	char linebuf[LINEBUF_SIZE];

	if(fgets(linebuf, LINEBUF_SIZE, stdin) == NULL) {
		RETURN(0);
	}

	size_t linebuf_len = strlen(linebuf);
	if(linebuf[linebuf_len - 1] == '\n') { // whole line was read
		linebuf[linebuf_len - 1] = ' ';

		char* space_pos = strchr(linebuf, ' ');
		*space_pos = '\0';

		size_t len = space_pos - linebuf + 1;
		if(len > size) {
			RETURN(0);
		}

		memcpy(buf, linebuf, len);
		RETURN(1);
	}
	else { // overflow, need to clear stdin
		scanf("%*[^\n]");
		getc(stdin);

		RETURN(0);
	}
$$
}
int const read_line(char buf[], size_t size)
{$_
	ASSERT(buf != NULL);

	if(fgets(buf, size, stdin) == NULL) {
		RETURN(0);
	}

	size_t len = strlen(buf);
	if(buf[len - 1] != '\n') {
		scanf("%*[^\n]");
		getc(stdin);

		RETURN(0);
	}

	buf[len - 1] = '\0';

	RETURN(1);
$$
}

static void string_to_lower(char* string) 
{$_
	ASSERT(string != NULL);

	for(; *string != '\0'; ++string) {
		*string = (char)tolower((int)*string);
	}
$$
}


int const yes_or_no() 
{$_
	for(;;) {
		printf("Print \'yes\' if you are agree, \'no\' otherwise: ");
		fflush(stdout);

		char buf[4];
		if(read_word(buf, 4)) {
			string_to_lower(buf);

			if(strcmp(buf, "no") == 0) { 
				RETURN(0);
			}
			if(strcmp(buf, "yes") == 0) {
				RETURN(1);
			}
		}

	}
$$
}
