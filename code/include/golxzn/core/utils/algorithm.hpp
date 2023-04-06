#pragma once

#include <cassert>
#include <algorithm>
#include <golxzn/core/aliases.hpp>

namespace golxzn::core::utils {

template<class DestIter, class InputIter>
[[nodiscard]] constexpr DestIter fill_range(
		DestIter dest_begin, const DestIter dest_end,
		const InputIter value_begin, const InputIter value_end) {
	assert(std::distance(dest_begin, dest_end) % std::distance(value_begin, value_end) == 0 &&
		"Destination range size must be dividable by value range size");

	while (dest_begin < dest_end) {
		dest_begin = std::copy(value_begin, value_end, dest_begin);
	}
	return dest_begin;
}

template<class DestIter, class InputIter>
[[nodiscard]] constexpr DestIter fill_range_n(DestIter dest, const usize count, const InputIter value, const usize value_count) {
	assert(count % value_count == 0 && "Count must be dividable by value_count");

	const auto dest_end{ dest + count };
	while (dest < dest_end) {
		dest = std::copy_n(value, value_count, dest);
	}
	return dest;
}



} // namespace golxzn::core::utils
