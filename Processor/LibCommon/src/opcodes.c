#include <ttrack/dbg.h>
#include "opcodes.h"


char const* opcode_str(opcode_t opcode)
{$_
	ASSERT(opcode < OPCODES_COUNT);

#ifdef COOL_MODE
	char const* TABLE[OPCODES_COUNT] = {
		"забудь",
		"расплатись",
		"сдача",
		"прибавь",
		"отними",
		"лошадь",
		"раздупли",
		"грех",
		"сероокись", // Carbon Oxide Sulphide
		"корень",
		"возложи",
		"возложи",
		"прибери",
		"прибери",
		"вспомни",
		"помири",
		"раззадорь",
		"принизь",
		"возвысь",
		"неунизь",
		"невозвысь",
		"припомни",
		"отпусти"
	};
#else
	char const* TABLE[OPCODES_COUNT] = {
		"hlt",
		"in",
		"out",
		"add",
		"sub",
		"mul",
		"div",
		"sin",
		"cos",
		"sqrt",
		"push",
		"push",
		"pop",
		"pop",
		"jmp",
		"je",
		"jn",
		"jl",
		"jg",
		"jge",
		"jle",
		"call",
		"ret"
	};
#endif

	return TABLE[opcode];
$$
}
