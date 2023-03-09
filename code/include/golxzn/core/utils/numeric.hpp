#pragma once

#include <cmath>
#include <limits>
#include <golxzn/core/utils/traits.hpp>

namespace golxzn::core::utils::numeric {

template<class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
struct get_epsilon { static constexpr T value{ std::numeric_limits<T>::epsilon() }; };

template<class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
struct get_lowest { static constexpr T value{ std::numeric_limits<T>::lowest() }; };

template<class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
struct get_max { static constexpr T value{ std::numeric_limits<T>::max() }; };

template<class T> static constexpr T get_epsilon_v{ get_epsilon<T>::value };
template<class T> static constexpr T get_lowest_v{ get_lowest<T>::value };
template<class T> static constexpr T get_max_v{ get_max<T>::value };


namespace approximately {

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
[[nodiscard]] constexpr bool are_equal(const T a, const T b, const T tolerance = get_epsilon_v<T>) noexcept {
	if (const T diff{ std::abs(a - b) };
		diff <= tolerance || diff < std::max(std::abs(a), std::abs(b)) * tolerance) {
		return true;
	}
	return false;
}

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
[[nodiscard]] constexpr bool is_zero(const T a, const T tolerance = get_epsilon_v<T>) noexcept {
	return std::abs(a) <= tolerance;
}

} // namespace approximately

namespace definitely {

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
[[nodiscard]] constexpr bool is_less(const T a, const T b, const T tolerance = get_epsilon_v<T>) noexcept {
	if (const T diff{ a - b };
		diff < tolerance || diff < std::max(std::abs(a), std::abs(b)) * tolerance) {
		return true;
	}

	return false;
}

template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
[[nodiscard]] constexpr bool is_greater(const T a, const T b, const T tolerance = get_epsilon_v<T>) noexcept {
	if (const T diff{ a - b };
		diff > tolerance || diff > std::max(std::abs(a), std::abs(b)) * tolerance) {
		return true;
	}

	return false;
}

} // namespace definitely


// implements ULP method
// use this when you are only concerned about floating point precision issue
// for example, if you want to see if a is 1.0 by checking if its within
// 10 closest representable floating point numbers around 1.0.
template<typename T, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
[[nodiscard]] constexpr bool is_within_precision_interval(const T a, const T b, const u32 interval_size = 1_u32) noexcept {
	const T min_a{ a - (a - std::nextafter(a, get_lowest_v<T>)) * interval_size };
	const T max_a{ a + (std::nextafter(a, get_max_v<T>) - a) * interval_size };

	return min_a <= b && max_a >= b;
}


template<typename T, std::enable_if_t<std::is_integral_v<T>, bool> = false>
[[nodiscard]] constexpr T clamp(const T value, const T min, const T max) noexcept {
	return definitely::is_less(value, min) ? min : definitely::is_greater(value, max) ? max : value;
}


} // namespace golxzn::core::utils::numeric
