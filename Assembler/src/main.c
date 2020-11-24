#include <ttrack/dbg.h>
#include <ttrack/binbuf.h>
#include <libcommon/labeldic.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void signal_sigsegv(int _) {
	stacktrace_print(stderr);
	abort();
}

#include "parser.h"

int main(int argc, char* argv[])
{$_
	signal(SIGSEGV, signal_sigsegv);

	if(argc != 3) {
		fprintf(stderr, "[ERROR] Expected \'assembler <input_file> <output file>\'"
				"format\n");
		exit(EXIT_FAILURE);
	}

	binbuf_err_t binbuferr = binbuf_init(64);
	if(binbuferr != BINBUF_ERR_OK) {
		fprintf(stderr, "Fatal error occured while binbuf initialization: %s\n",
				binbuf_errstr(binbuferr));
		exit(EXIT_FAILURE);
	}

	if(!parser_init()) {
		exit(EXIT_FAILURE);
	}

	if(!parser_pass(argv[1], 0) || 
	   (binbuf_reset(), !parser_pass(argv[1], 1))) {
		exit(EXIT_FAILURE);
	}

	parser_free();

	FILE* ofile = fopen(argv[2], "wb");
	if(ofile == NULL || binbuf_flush(ofile) != BINBUF_ERR_OK) {
		fprintf(stderr, "[ERROR] Failed to write output file\n");
	}
	fclose(ofile);

	binbuf_free();
$$
}
