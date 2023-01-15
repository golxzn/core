#pragma once

#include <limits>

#include <golxzn/core/types.hpp>

namespace golxzn::core {

constexpr u8 invalid_id_u8{ std::numeric_limits<u8>::max() };
constexpr u16 invalid_id_u16{ std::numeric_limits<u16>::max() };
constexpr u32 invalid_id_u32{ std::numeric_limits<u32>::max() };
constexpr u64 invalid_id_u64{ std::numeric_limits<u64>::max() };

} // namespace golxzn::core

