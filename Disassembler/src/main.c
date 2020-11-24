#ifdef __GNUC__
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ttrack/binbuf.h>
#include <ttrack/dbg.h>
#include <libcommon/labeldic.h>

#include "disasm.h"
#include "disasmerr.h"

int main(int argc, char* argv[]) 
{$_
	if(argc != 3) {
		fprintf(stderr, 
				"[ERROR] Excepted format \'disassembler <input_file> <output_file>\'\n");
		RETURN(0);
	}

	FILE* ifile = fopen(argv[1], "rb");
	if(ifile == NULL) {
		fprintf(stderr, "[ERROR] File \'%s\' not found\n", argv[1]);
		RETURN(0);
	}

	if(binbuf_initf(ifile) != BINBUF_ERR_OK) {
		fclose(ifile);
		fprintf(stderr, "[ERROR] Failed to read input file\n");
		RETURN(0);
	}
	
	fclose(ifile);

	binbuf_reset();

	FILE* ofile = fopen(argv[2], "w");

	if(!disasm_pass(NULL, 0)) {
		disasm_err_print_this();
		goto finish;
	}

	binbuf_reset();
	if(!disasm_pass(ofile, 1)) {
		disasm_err_print_this();
		goto finish;
	}

finish: // free all resources
	fclose(ofile);
	labeldic_free();
	binbuf_clearerr();
	binbuf_free();
$$
}
