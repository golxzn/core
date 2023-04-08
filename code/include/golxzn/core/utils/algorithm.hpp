#pragma once

#include <cassert>
#include <algorithm>
#include <golxzn/core/aliases.hpp>

namespace golxzn::core::utils {

template<class DestIter, class InputIter>
constexpr DestIter fill_range(
		DestIter dest_begin, const DestIter dest_end,
		InputIter value_begin, const InputIter value_end) {
	assert(std::distance(dest_begin, dest_end) % std::distance(value_begin, value_end) == 0 &&
		"Destination range size must be dividable by value range size");

	while (dest_begin < dest_end) {
		dest_begin = std::copy(value_begin, value_end, dest_begin);
	}
	return dest_begin;
}

template<class DestIter, class InputIter>
constexpr DestIter fill_range_n(DestIter dest, const usize count, InputIter value, const usize value_count) {
	assert(count % value_count == 0 && "Count must be dividable by value_count");

	const auto dest_end{ dest + count };
	while (dest < dest_end) {
		dest = std::copy_n(value, value_count, dest);
	}
	return dest;
}

template<class DestIter, class InputIter>
constexpr DestIter copy_n_m(InputIter first, const usize count, const usize value_chunk, const size diff, DestIter dest) {
	const auto dest_end{ dest + count };
	if (diff < 0) {
		while(dest != dest_end) {
			dest = std::copy_n(first, value_chunk + diff, dest);
			first += value_chunk;
		}
		return dest;
	}

	if (diff > 0) {
		while(dest != dest_end) {
			dest = std::copy_n(first, value_chunk, dest) + diff;
			first += value_chunk;
		}
		return dest;
	}
	return std::copy_n(first, count, dest);
}

} // namespace golxzn::core::utils
