#ifdef __GNUC__
#	pragma GCC diagnostic ignored "-Wignored-qualifiers"
#	pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <libcommon/reginfo.h>
#include <libcommon/opcodes.h>

#define STACK_DATA_T double
#define STACK_DATA_PRINTF_SEQ "%lf"
#include <ttrack/stack.h>
#undef STACK_DATA_T
#undef STACK_DATA_PRINTF_SEQ

#define STACK_DATA_T size_t
#define STACK_DATA_PRINTF_SEQ "%zu"
#include <ttrack/stack.h>
#undef STACK_DATA_T
#undef STACK_DATA_PRINTF_SEQ

#include <ttrack/dbg.h>
#include <ttrack/binbuf.h>
#include <ttrack/comp.h>

#define EPS 1e-7

#define MEM_SIZE 1024

double regs[REGCNT];
stack_double_t stack;
stack_size_t_t callstack;
double mem[MEM_SIZE];

int const cpu_init(char const* binfile)
{$_
	FILE* ifile = fopen(binfile, "rb");
	if(ifile == NULL) {
		fprintf(stderr, "[ERROR] File \'%s\' not found\n", binfile);
		RETURN(0);
	}

	if(binbuf_initf(ifile) != BINBUF_ERR_OK) {
		fclose(ifile);
		fprintf(stderr, "[ERROR] Failed to read input file\n");
		RETURN(0);
	}

	fclose(ifile);
	binbuf_reset();

	if(stack_init(double, &stack, 4) != STACK_ERR_OK ||
	   stack_init(size_t, &callstack, 4) != STACK_ERR_OK) {
		binbuf_free();
		fprintf(stderr, "[ERROR] Failed to allocate stack and callstack\n");
		RETURN(0);
	}

	RETURN(1);
}
void cpu_free()
{$_
	stack_free(double, &stack);
	stack_free(size_t, &callstack);
	binbuf_free();
$$
}

#define STACK_CHECK(expr) 											\
	if((serr = expr) != STACK_ERR_OK) {								\
		fprintf(stderr, "stack error: %s\n", stack_errstr(serr));	\
		$$															\
		return;														\
	}

#define BINBUF_CHECK(expr)											\
	if((berr = expr) != BINBUF_ERR_OK) {							\
		binbuf_clearerr();											\
		fprintf(stderr, "binbuf error at %zu (opcode %hhu): %s\n", 	\
				binbuf_pos(), opcode,  binbuf_errstr(berr));		\
		$$															\
		return;														\
	}

#define REGID_CHECK() 												\
	if(id >= REGCNT) {												\
		fprintf(stderr, "invalid register id %hhu\n", id);			\
		$$															\
		return;														\
	}

#define MEM_CHECK()													\
	if(regs[id] < 0 || (size_t)regs[id] + pos >= MEM_SIZE) {		\
		fprintf(stderr, "segmentation violation\n");				\
		$$															\
		return;														\
	}

void cpu_execute()
{$_
	opcode_t opcode;
	stack_err_t serr;
	binbuf_err_t berr;
	double op1, op2;
	offset_t pos;
	size_t spos;
	regid_t id;

	while(binbuf_read_value(opcode_t, opcode) != BINBUF_ERR_RANGE) {
		switch(opcode) {
		case OPCODE_HLT:
			STACKTRACE_POP
			return;
			break;

		case OPCODE_IN:
			fprintf(stdout, "double value: ");
			while(scanf("%lf", &op1) != 1) {
				scanf("%*[^\n]");
				fprintf(stdout, "invalid format, try again: ");
			}
			STACK_CHECK(stack_push(double, &stack, op1));
			break;

		case OPCODE_OUT:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			fprintf(stdout, "out: %lf\n", op1);
			break;

		case OPCODE_ADD:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			STACK_CHECK(stack_push(double, &stack, op1 + op2));
			break;

		case OPCODE_SUB:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			STACK_CHECK(stack_push(double, &stack, op1 - op2));
			break;

		case OPCODE_MUL:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			STACK_CHECK(stack_push(double, &stack, op1 * op2));
			break;

		case OPCODE_DIV:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			STACK_CHECK(stack_push(double, &stack, op1 / op2));
			break;

		case OPCODE_SIN:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_push(double, &stack, sin(op1)));
			break;

		case OPCODE_COS:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_push(double, &stack, cos(op1)));
			break;

		case OPCODE_SQRT:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_push(double, &stack, sqrt(op1)));
			break;

		case OPCODE_PUSHV:
			BINBUF_CHECK(binbuf_read_value(double, op1));
			STACK_CHECK(stack_push(double, &stack, op1));
			break;

		case OPCODE_PUSHR:
			BINBUF_CHECK(binbuf_read_value(regid_t, id));
			REGID_CHECK();
			STACK_CHECK(stack_push(double, &stack, regs[id]));
			break;

		case OPCODE_PUSHM:
			BINBUF_CHECK(binbuf_read_value(regid_t, id));
			REGID_CHECK();
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			MEM_CHECK();
			STACK_CHECK(stack_push(double, &stack, mem[(size_t)regs[id] + pos]));
			break;

		case OPCODE_POPV:
			STACK_CHECK(stack_pop(double, &stack, &op1));
			break;

		case OPCODE_POPR:
			BINBUF_CHECK(binbuf_read_value(regid_t, id));
			REGID_CHECK();
			STACK_CHECK(stack_pop(double, &stack, regs + id));
			break;

		case OPCODE_POPM:
			BINBUF_CHECK(binbuf_read_value(regid_t, id));
			REGID_CHECK();
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			MEM_CHECK();
			STACK_CHECK(stack_pop(double, &stack, mem + (size_t)regs[id] + pos));
			break;

		case OPCODE_JMP:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			BINBUF_CHECK(binbuf_seek((size_t)pos));
			break;

		case OPCODE_JE:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			if(about(op1, op2, EPS)) {
				BINBUF_CHECK(binbuf_seek((size_t)pos));
			}
			break;

		case OPCODE_JN:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			if(!about(op1, op2, EPS)) {
				BINBUF_CHECK(binbuf_seek((size_t)pos));
			}
			break;

		case OPCODE_JL:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			if(op1 < op2) {
				BINBUF_CHECK(binbuf_seek((size_t)pos));
			}
			break;

		case OPCODE_JG:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			fprintf(stderr, "%lf > %lf?\n", op1, op2);
			if(op1 > op2) {
				BINBUF_CHECK(binbuf_seek((size_t)pos));
			}
			break;
		case OPCODE_JGE:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			if(op1 >= op2) {
				BINBUF_CHECK(binbuf_seek((size_t)pos));
			}
			break;
		case OPCODE_JLE:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			STACK_CHECK(stack_pop(double, &stack, &op1));
			STACK_CHECK(stack_pop(double, &stack, &op2));
			if(op1 <= op2) {
				BINBUF_CHECK(binbuf_seek((size_t)pos));
			}
			break;

		case OPCODE_CALL:
			BINBUF_CHECK(binbuf_read_value(offset_t, pos));
			STACK_CHECK(stack_push(size_t, &callstack, binbuf_pos()));
			BINBUF_CHECK(binbuf_seek((size_t)pos));
			break;

		case OPCODE_RET:
			STACK_CHECK(stack_pop(size_t, &callstack, &spos));
			BINBUF_CHECK(binbuf_seek(spos));
			break;

		default:
			fprintf(stderr, "unknown command %hhu\n", opcode);
			$$
			break;
		}
	}		

$$
}

int main(int argc, char* argv[])
{$_
	if(argc != 2) {
		fprintf(stderr, "[ERROR] Excepted format \'emulator <input_file>\'\n");
		RETURN(EXIT_FAILURE);
	}

	if(!cpu_init(argv[1])) {
		RETURN(EXIT_FAILURE);
	}

	cpu_execute();
	cpu_free();

}
