#pragma once

#include <golxzn/core/types.hpp>
#include <golxzn/core/traits.hpp>

namespace golxzn::core::utils {

template<class ...Args>
[[nodiscard]] constexpr size_t size(const Args &...args) noexcept {
	return ((std::size(args) - 1) + ...) + 1;
}

template<>
[[nodiscard]] constexpr size_t size() noexcept {
	return {};
}

} // namespace golxzn::core::utils
