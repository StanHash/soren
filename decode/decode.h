#ifndef SOREN_DECODE_INCLUDED
#define SOREN_DECODE_INCLUDED

#include <cstdint>

#include "core/types.h"
#include "core/soren-cmb.h"

namespace soren {

using byte_type = std::uint8_t;

CmbInfo decode_cmb(Span<const byte_type> data, GameKind game);

} // namespace soren

#endif // SOREN_DECODE_INCLUDED
