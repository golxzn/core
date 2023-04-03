#pragma once

#include <chrono>
#include "golxzn/core/export.hpp"
#include "golxzn/core/types.hpp"

namespace golxzn::core {

class GOLXZN_EXPORT time {
public:
	constexpr time() noexcept = default;

	template<class Rep, class Period = std::ratio<1>>
	constexpr explicit time(const std::chrono::duration<Rep, Period> duration) noexcept
		: m_microseconds{ duration } {}

	template<class T = f32, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
	[[nodiscard]] constexpr T seconds() const noexcept {
		return std::chrono::duration<T>{ m_microseconds }.count();
	}

	template<class T = i32, std::enable_if_t<std::is_integral_v<T>, bool> = false>
	[[nodiscard]] constexpr T milliseconds() const noexcept {
		return std::chrono::duration<T, std::milli>{ m_microseconds }.count();
	}

	template<class T = i64, std::enable_if_t<std::is_integral_v<T>, bool> = false>
	[[nodiscard]] constexpr T microseconds() const noexcept {
		return m_microseconds.count();
	}

	static constexpr time zero() noexcept { return time{}; }

	template<class T = f32, std::enable_if_t<std::is_floating_point_v<T>, bool> = false>
	[[nodiscard]] static constexpr time seconds(const T seconds) noexcept {
		return time{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<T>{ seconds }) };
	}

	template<class T = i32, std::enable_if_t<std::is_integral_v<T>, bool> = false>
	[[nodiscard]] static constexpr time milliseconds(const T milliseconds) noexcept {
		return time{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<T, std::milli>{ milliseconds }) };
	}

	template<class T = i64, std::enable_if_t<std::is_integral_v<T>, bool> = false>
	[[nodiscard]] static constexpr time microseconds(const T microseconds) noexcept {
		return time{ std::chrono::duration<T>{ microseconds } };
	}


	[[nodiscard]] constexpr bool operator==(const time& r) const noexcept { return m_microseconds == r.m_microseconds; }
	[[nodiscard]] constexpr bool operator!=(const time& r) const noexcept { return !(*this == r); }
	[[nodiscard]] constexpr bool operator<(const time& r) const noexcept { return m_microseconds < r.m_microseconds; }
	[[nodiscard]] constexpr bool operator>(const time& r) const noexcept { return r < *this; }
	[[nodiscard]] constexpr bool operator<=(const time& r) const noexcept { return !(*this > r); }
	[[nodiscard]] constexpr bool operator>=(const time& r) const noexcept { return !(*this < r); }

	[[nodiscard]] constexpr time operator+(const time& r) const noexcept { return time{ m_microseconds + r.m_microseconds }; }
	[[nodiscard]] constexpr time operator-(const time& r) const noexcept { return time{ m_microseconds - r.m_microseconds }; }
	[[nodiscard]] constexpr time operator*(const time& r) const noexcept { return microseconds(microseconds() * r.microseconds()); }
	[[nodiscard]] constexpr time operator/(const time& r) const noexcept { return microseconds(microseconds() / r.microseconds()); }
	[[nodiscard]] constexpr time operator%(const time& r) const noexcept { return microseconds(microseconds() % r.microseconds()); }

	constexpr time& operator+=(const time& r) noexcept { return *this = *this + r; }
	constexpr time& operator-=(const time& r) noexcept { return *this = *this - r; }
	constexpr time& operator*=(const time& r) noexcept { return *this = *this * r; }
	constexpr time& operator/=(const time& r) noexcept { return *this = *this / r; }
	constexpr time& operator%=(const time& r) noexcept { return *this = *this % r; }

	[[nodiscard]] constexpr operator std::chrono::microseconds() const noexcept { return m_microseconds; }

	[[nodiscard]] constexpr time operator-() const noexcept { return time{ microseconds(-microseconds()) }; }

private:
	std::chrono::microseconds m_microseconds{};
};

} // namespace golxzn::core
