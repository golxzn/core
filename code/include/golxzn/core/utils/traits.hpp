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


template<class T>
struct container_value_type {
	using type = std::remove_reference_t<T>;
};

template<class T, std::size_t N>
struct container_value_type<T[N]> {
	using type = T;
};

template<class T>
struct container_value_type<T*> {
	using type = T;
};

template<class T, std::size_t N>
struct container_value_type<std::array<T, N>> {
	using type = T;
};

// Specializations for std::vector, std::list, std::deque, std::set, std::multiset, std::map, std::multimap
template<template<class...> class Container, class... Args>
struct container_value_type<Container<Args...>> {
	using type = typename Container<Args...>::value_type;
};

template<class T>
using container_value_type_t = typename container_value_type<T>::type;

} // namespace golxzn::core