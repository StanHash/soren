
#include "soren-bytecode.h"

#include <limits>

namespace soren {

static constexpr
const auto vardiff = std::numeric_limits<decltype(BcOpcodeInfo::stackDiff)>::max();

const BcOpcodeInfo gBcOpcodeInfo[BC_OPCODE_COUNT]
{
	{ "nop",     0, 0 },

	{ "val",    +1, 1 },
	{ "val",    +1, 2 },
	{ "valx",   +1, 1 },
	{ "valx",   +1, 2 },
	{ "valy",   +1, 1 },
	{ "valy",   +1, 2 },
	{ "ref",    +1, 1 },
	{ "ref",    +1, 2 },
	{ "refx",   +1, 1 },
	{ "refx",   +1, 2 },
	{ "refy",   +1, 1 },
	{ "refy",   +1, 2 },
	{ "gval",   +1, 1 },
	{ "gval",   +1, 2 },
	{ "gvalx",  +1, 1 },
	{ "gvalx",  +1, 2 },
	{ "gvaly",  +1, 1 },
	{ "gvaly",  +1, 2 },
	{ "gref",   +1, 1 },
	{ "gref",   +1, 2 },
	{ "grefx",  +1, 1 },
	{ "grefx",  +1, 2 },
	{ "grefy",  +1, 1 },
	{ "grefy",  +1, 2 },

	{ "number", +1, 1 },
	{ "number", +1, 2 },
	{ "number", +1, 4 },
	{ "string", +1, 1 },
	{ "string", +1, 2 },
	{ "string", +1, 4 },

	{ "deref",  +1, 0 },
	{ "disc",   -1, 0 },
	{ "store",  -1, 0 },
	{ "add",    -1, 0 },
	{ "sub",    -1, 0 },
	{ "mul",    -1, 0 },
	{ "div",    -1, 0 },
	{ "mod",    -1, 0 },
	{ "neg",     0, 0 },
	{ "mvn",     0, 0 },
	{ "not",     0, 0 },
	{ "orr",    -1, 0 },
	{ "and",    -1, 0 },
	{ "xor",    -1, 0 },
	{ "lsl",    -1, 0 },
	{ "lsr",    -1, 0 },
	{ "eq",     -1, 0 },
	{ "ne",     -1, 0 },
	{ "lt?",    -1, 0 },
	{ "le",     -1, 0 },
	{ "gt?",    -1, 0 },
	{ "ge?",    -1, 0 },
	{ "eqstr",  -1, 0 },
	{ "nestr",  -1, 0 },

	{ "call.", vardiff, 1 },
	{ "call", vardiff, 3 },
	{ "ret",     0, 0 },
	{ "b",       0, 2 },
	{ "by",     -1, 2 },
	{ "bky",  vardiff, 2 },
	{ "bn",     -1, 2 },
	{ "bkn",  vardiff, 2 },
	{ "yield",   0, 0 },

	{ "unk",     0, 4 },
	{ "printf", vardiff, 1 },

	{ "inc",    -1, 0 },
	{ "dec",    -1, 0 },
	{ "dup",    +1, 0 },
	{ "retn",    0, 0 },
	{ "rety",    0, 0 },
	{ "assign", -2, 0 },

	{ "scand",  -1, 0 }, // Short-Circuiting And
	{ "scorr",  -1, 0 }, // Short-Circuiting Orr
};

} // namespace soren
