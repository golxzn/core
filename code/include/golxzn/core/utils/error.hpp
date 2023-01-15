#pragma once

#if defined(GOLXZNC_NO_EXCEPTIONS)
#include <cstdlib>
#else
#include <exception>
#endif

#include <string>
#include <string_view>

namespace golxzn::core::utils::error {

#if defined(GOLXZNC_NO_EXCEPTIONS)
static constexpr bool exceptions_enabled{ false };
#else
static constexpr bool exceptions_enabled{ true };
#endif

template<class T>
[[noreturn]] void out_of_range([[maybe_unused]] T error) {
#if !defined(GOLXZNC_NO_EXCEPTIONS)
	throw std::out_of_range{ std::forward<T>(error) };
#else
	std::abort();
#endif
}

template<class T>
[[noreturn]] void runtime_error([[maybe_unused]] T error) {
#if !defined(GOLXZNC_NO_EXCEPTIONS)
	throw std::runtime_error{ std::forward<T>(error) };
#else
	std::abort();
#endif
}

} // namespace golxzn::core::utils::error
