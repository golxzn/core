#pragma once

#include <golxzn/core/aliases.hpp>

namespace golxzn::core::traits {

template<class T, class ...Ts>
struct is_any_of : std::bool_constant<(std::is_same_v<T, Ts> || ...)> {};

template<class T, class ...Ts>
constexpr bool is_any_of_v{ is_any_of<T, Ts...>::value };

template<class T, class ...Ts>
struct are_same : std::bool_constant<(std::is_same_v<T, Ts> && ...)> {};

template<class T, class ...Ts>
constexpr bool are_same_v{ are_same<T, Ts...>::value };

template<class T, class ...Ts>
struct are_convertible : std::bool_constant<(std::is_convertible_v<T, Ts> && ...)> {};

template<class T, class ...Ts>
constexpr bool are_convertible_v{ are_same<T, Ts...>::value };

template<class T, class ...Ts>
[[nodiscard]] constexpr bool any_from(const T &t, const Ts &...ts) {
	return ((t == ts) || ...);
}

template<class T, class ...Ts>
[[nodiscard]] constexpr bool none_from(const T &t, const Ts &...ts) {
	return ((t != ts) && ...);
}

template<class T, class ...Ts>
[[nodiscard]] constexpr bool all_from(const T &t, const Ts &...ts) {
	return ((t == ts) && ...);
}

} // namespace golxzn::core