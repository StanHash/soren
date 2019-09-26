
#include "decode/decode.h"

namespace soren {

enum
{
	GLOBAL_AMT_SUSPICION_LIMIT = 1000,
	LOCALS_AMT_SUSPICION_LIMIT = 1000,
	PARAMS_AMT_SUSPICION_LIMIT = 20,
};

template<typename IteratorType, typename ResultType = std::uint32_t>
static inline
ResultType decode_int_le(IteratorType begin, IteratorType end)
{
	static_assert(std::is_convertible<typename std::iterator_traits<IteratorType>::value_type, byte_type>::value, "decode_le: expected byte (u8) iterators");

	ResultType result = 0;
	unsigned i = 0;

	while (begin != end)
		result = result + (*begin++ << (8*i++));

	return result;
}

template<typename IteratorType, typename ResultType = std::uint32_t>
static inline
ResultType decode_int_be(IteratorType begin, IteratorType end)
{
	static_assert(std::is_convertible<typename std::iterator_traits<IteratorType>::value_type, byte_type>::value, "decode_le: expected byte (u8) iterators");

	ResultType result = 0;

	while (begin != end)
		result = *begin++ + (result << 8);

	return result;
}

template<typename ResultType = std::uint32_t>
static inline
ResultType decode_int_le(Span<const byte_type> span)
{
	return decode_int_le<decltype(span.begin()), ResultType>(span.begin(), span.end());
}

template<typename ResultType = std::uint32_t>
static inline
ResultType decode_int_be(Span<const byte_type> span)
{
	return decode_int_be<decltype(span.begin()), ResultType>(span.begin(), span.end());
}

template<typename IntType = std::int32_t>
static inline
IntType sign_extend(IntType value, unsigned bits)
{
	static_assert(std::is_signed<IntType>::value, "Result of sign_extend should be signed");

	const auto rbits = (sizeof(IntType)*8 - bits);

	return (value << rbits) >> rbits;
}

std::vector<BcIns> decode_script(Span<const byte_type> data, GameKind game)
{
	std::vector<BcIns> result;

	unsigned i = 0, lastJump = 0;
	bool ended = false;

	while (!ended && i < data.size())
	{
		BcIns ins { i, 0, 0 };
		ins.opcode = data[i++];

		if (!ins.valid(game))
			throw std::runtime_error("Invalid opcode."); // TODO: better error

		if (ins.info().operandSize > 0)
		{
			if (i + ins.info().operandSize > data.size())
				throw std::runtime_error("Reached end of script when expecting operand."); // TODO: better error

			ins.operand = decode_int_be(
				data.begin() + i,
				data.begin() + i + ins.info().operandSize);

			ins.operand = sign_extend(ins.operand, ins.info().operandSize*8);

			i += ins.info().operandSize;

			if ((game == GameKind::FE10) && (ins.opcode == BC_OPCODE_CALL))
			{
				// in FE10 only, call(37) has a variable length operand
				// if the first byte of the operand is >= 0x80
				// the operand will be 2 bytes be, with the top bit removed

				if (ins.operand & 0x80)
				{
					if (i >= data.size())
						throw std::runtime_error("Reached end of script when expecting operand."); // TODO: better error

					ins.operand = ((ins.operand & 0x7F) << 8) + data[i++];
				}
			}
		}

		switch (ins.opcode)
		{

		case BC_OPCODE_B:
		case BC_OPCODE_BY:
		case BC_OPCODE_BKY:
		case BC_OPCODE_BN:
		case BC_OPCODE_BKN:
			ins.operand = i + ins.operand - ins.info().operandSize;
			lastJump = std::max(lastJump, (unsigned) ins.operand);

			break;

		case BC_OPCODE_RETURN:
		case BC_OPCODE_RETN:
		case BC_OPCODE_RETY:
			if (i > lastJump)
				ended = true;

			break;

		} // switch (ins.opcode)

		result.push_back(ins);
	}

	if (result.empty() || !result.back().is_end())
		throw std::runtime_error("Reached end of file without reached end of script.");

	return result;
}

CmbInfo decode_cmb(Span<const byte_type> data, GameKind game)
{
	CmbInfo result;

	if (data.size() < 0x2C)
		throw std::runtime_error("This is not a valid CMB file! (too small)"); // TODO: better error

	// 1. Read cmb information

	const auto globalAmt  = decode_int_le(data.subspan(0x22, 2));
	const auto offStrings = decode_int_le(data.subspan(0x24, 4));
	const auto offEvents  = decode_int_le(data.subspan(0x28, 4));

	if (offStrings >= data.size())
		throw std::runtime_error("String pool past the end of the file!"); // TODO: better error

	if (offEvents >= data.size())
		throw std::runtime_error("Event offset array past the end of the file!"); // TODO: better error

	if (globalAmt > GLOBAL_AMT_SUSPICION_LIMIT)
		throw std::runtime_error("CMB global variable amount is past the suspicion limit!"); // TODO: better error

	// String pool
	result.stringPool.assign(
		data.begin() + offStrings,
		(offStrings > offEvents)
			? data.end()
			: data.begin() + offEvents);

	// Global variables
	result.globalNames.resize(globalAmt);

	for (unsigned i = 0; i < globalAmt; ++i)
	{
		result.globalNames[i] = [&] ()
		{
			// TODO: less hacky way of doing this
			std::string r("gvar_");
			r.append(std::to_string(i));

			return r;
		} ();
	}

	// 2. Read scene information

	for (unsigned i = 0;; ++i)
	{
		if (offEvents + i*4 + 4 > data.size())
			throw std::runtime_error("Event offset array unterminated by then end of the file"); // TODO: better error

		const auto offEvent = decode_int_le(data.subspan(offEvents + 4*i, 4));

		if (offEvent == 0)
			break; // We reached the end!

		if (offEvent + 0x14 > data.size())
			throw std::runtime_error("Scene information goes past the end of the file"); // TODO: better error

		const auto offName   = decode_int_le(data.subspan(offEvent + 0x00, 4));
		const auto offScript = decode_int_le(data.subspan(offEvent + 0x04, 4));
		const auto kind      = decode_int_le(data.subspan(offEvent + 0x0C, 1));
		const auto argAmt    = decode_int_le(data.subspan(offEvent + 0x0D, 1));
		const auto paramAmt  = decode_int_le(data.subspan(offEvent + 0x0E, 1));
		const auto idx       = decode_int_le(data.subspan(offEvent + 0x10, 2));
		const auto varAmt    = decode_int_le(data.subspan(offEvent + 0x12, 2));

		if (paramAmt > PARAMS_AMT_SUSPICION_LIMIT)
			throw std::runtime_error("Scene parameter amount is past the suspicion limit!"); // TODO: better error

		if (varAmt > LOCALS_AMT_SUSPICION_LIMIT)
			throw std::runtime_error("Scene variable amount is past the suspicion limit!"); // TODO: better error

		if (argAmt > varAmt)
			throw std::runtime_error("Scene argument amount is past the variable amount!"); // TODO: better error

		if (offEvent + 0x14 + 2*paramAmt > data.size())
			throw std::runtime_error("Scene information parameters goes past the end of the file"); // TODO: better error

		if (idx != i)
			throw std::runtime_error("Scene information is invalid (index doesn't match)!"); // TODO: better error

		result.scenes.emplace_back();
		auto& scene = result.scenes.back();

		scene.idx      = idx;
		scene.kind     = kind;
		scene.argCnt   = argAmt;
		scene.isGlobal = (offName != 0);

		// Read name
		scene.name = [&] ()
		{
			if (offName == 0)
				return [&] () { std::string r("Unknown_"); r.append(std::to_string(idx)); return r; } (); // TODO: better string formatting

			std::string result;

			for (unsigned i = offName;; ++i)
			{
				if (i > data.size())
					throw std::runtime_error("Scene name string reaches past the end of the file");

				if (data[i] == 0)
					break;

				result.push_back(data[i]);
			}

			return result;
		} ();

		// Read parameters
		scene.parameters = [&] ()
		{
			std::vector<int> result(paramAmt, 0);

			for (unsigned i = 0; i < paramAmt; ++i)
				result[i] = decode_int_le(data.subspan(offEvent + 0x14 + 2*i, 2));

			return result;
		} ();

		// Name variables lazy names
		scene.varnames = [&] ()
		{
			std::vector<std::string> result(varAmt);

			for (unsigned i = 0; i < argAmt; ++i)
				result[i] = [&] () { std::string r("arg_"); r.append(std::to_string(i)); return r; } (); // TODO: better string formatting

			for (unsigned i = argAmt; i < varAmt; ++i)
				result[i] = [&] () { std::string r("var_"); r.append(std::to_string(i)); return r; } (); // TODO: better string formatting

			return result;
		} ();

		// Decode script
		scene.rawScript = decode_script(data.subspan(offScript), game);
	}

	return result;
}

} // namespace soren
