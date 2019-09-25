#ifndef SOREN_POR_BYTECODE_INCLUDED
#define SOREN_POR_BYTECODE_INCLUDED

#include <string>
#include <cstdint>

#include "core/types.h"

namespace soren {

enum
{
	BC_OPCODE_NOP      = 0x00,

	// Memory addressing
	BC_OPCODE_VAL8     = 0x01, // push [l+imm]
	BC_OPCODE_VAL16    = 0x02, // push [l+imm]
	BC_OPCODE_VALX8    = 0x03, // push [l+imm+@0]
	BC_OPCODE_VALX16   = 0x04, // push [l+imm+@0]
	BC_OPCODE_VALY8    = 0x05, // push [[l+imm]+@0]
	BC_OPCODE_VALY16   = 0x06, // push [[l+imm]+@0]
	BC_OPCODE_REF8     = 0x07, // push l+imm
	BC_OPCODE_REF16    = 0x08, // push l+imm
	BC_OPCODE_REFX8    = 0x09, // push l+imm+@0
	BC_OPCODE_REFX16   = 0x0A, // push l+imm+@0
	BC_OPCODE_REFY8    = 0x0B, // push [l+imm]+@0
	BC_OPCODE_REFY16   = 0x0C, // push [l+imm]+@0
	BC_OPCODE_GVAL8    = 0x0D, // same as 0x01 but with g instead of l
	BC_OPCODE_GVAL16   = 0x0E, // ...
	BC_OPCODE_GVALX8   = 0x0F,
	BC_OPCODE_GVALX16  = 0x10,
	BC_OPCODE_GVALY8   = 0x11,
	BC_OPCODE_GVALY16  = 0x12,
	BC_OPCODE_GREF8    = 0x13,
	BC_OPCODE_GREF16   = 0x14,
	BC_OPCODE_GREFX8   = 0x15,
	BC_OPCODE_GREFX16  = 0x16,
	BC_OPCODE_GREFY8   = 0x17,
	BC_OPCODE_GREFY16  = 0x18,

	// Constant loading
	BC_OPCODE_NUMBER8  = 0x19, // push imm
	BC_OPCODE_NUMBER16 = 0x1A, // push imm
	BC_OPCODE_NUMBER32 = 0x1B, // push imm
	BC_OPCODE_STRING8  = 0x1C, // push strpool+imm
	BC_OPCODE_STRING16 = 0x1D, // push strpool+imm
	BC_OPCODE_STRING32 = 0x1E, // push strpool+imm

	// Operations
	BC_OPCODE_DEREF    = 0x1F, // push [top]
	BC_OPCODE_DISC     = 0x20, // pop
	BC_OPCODE_STORE    = 0x21, // push a = b
	BC_OPCODE_ADD      = 0x22, // push a + b
	BC_OPCODE_SUB      = 0x23, // push a - b
	BC_OPCODE_MUL      = 0x24, // push a * b
	BC_OPCODE_DIV      = 0x25, // push a / b
	BC_OPCODE_MOD      = 0x26, // push a % b
	BC_OPCODE_NEG      = 0x27, // push -a
	BC_OPCODE_MVN      = 0x28, // push ~a
	BC_OPCODE_NOT      = 0x29, // push !a
	BC_OPCODE_ORR      = 0x2A, // push a | b
	BC_OPCODE_AND      = 0x2B, // push a & b
	BC_OPCODE_XOR      = 0x2C, // push a ^ b
	BC_OPCODE_LSL      = 0x2D, // push a << b
	BC_OPCODE_LSR      = 0x2E, // push a >> b
	BC_OPCODE_EQ       = 0x2F, // push a == b
	BC_OPCODE_NE       = 0x30, // push a != b
	BC_OPCODE_LT       = 0x31, // push a < b (unsure)
	BC_OPCODE_LE       = 0x32, // push a <= b
	BC_OPCODE_GT       = 0x33, // push a > b (unsure)
	BC_OPCODE_GE       = 0x34, // push a >= b (unsure)
	BC_OPCODE_EQSTR    = 0x35, // push strcmp(a, b) == 0
	BC_OPCODE_NESTR    = 0x36, // push strcmp(a, b) != 0

	// Jumps and calls
	BC_OPCODE_CALL     = 0x37, // push call by event idx
	BC_OPCODE_CALLEXT  = 0x38, // push call by name
	BC_OPCODE_RETURN   = 0x39, // return a
	BC_OPCODE_B        = 0x3A, // branch
	BC_OPCODE_BY       = 0x3B, // branch if yes
	BC_OPCODE_BKY      = 0x3C, // branch and keep if yes
	BC_OPCODE_BN       = 0x3D, // branch if no
	BC_OPCODE_BKN      = 0x3E, // branch and keep if no
	BC_OPCODE_YIELD    = 0x3F, // yield

	// Debug (dummied)
	BC_OPCODE_40       = 0x40,
	BC_OPCODE_PRINTF   = 0x41,

	BC_OPCODE_FE9_COUNT,

	// FE10 only
	BC_OPCODE_INC      = 0x42, // [a] = [a] + 1
	BC_OPCODE_DEC      = 0x43, // [a] = [a] - 1
	BC_OPCODE_DUP      = 0x44, // push top
	BC_OPCODE_RETN     = 0x45, // return 0
	BC_OPCODE_RETY     = 0x46, // return 1
	BC_OPCODE_ASSIGN   = 0x47, // a = b

	BC_OPCODE_FE10_COUNT,

	// Fake opcodes for simplified processing
	BC_FAKEOP_LAND = 0x48, // fake logical and instruction, generated from bkn
	BC_FAKEOP_LORR = 0x49, // fake logical or instruction, generated from bky

	BC_OPCODE_COUNT,
};

struct BcOpcodeInfo
{
	const char* mnemonic;
	int stackDiff;
	unsigned operandSize;
	bool isJump = false;
};

extern const BcOpcodeInfo gBcOpcodeInfo[BC_OPCODE_COUNT];

template<typename LocationType>
struct BcIns
{
	inline const BcOpcodeInfo& info() const { return gBcOpcodeInfo[opcode]; }

	template<bool IsFE10>
	inline bool valid() const
	{
		return IsFE10
			? opcode < BC_OPCODE_FE10_COUNT
			: opcode < BC_OPCODE_FE9_COUNT;
	}

	LocationType location;

	std::int32_t operand;
	std::uint8_t opcode;
};

} // namespace soren

#endif // SOREN_POR_BYTECODE_INCLUDED
