#pragma once

#include <optional>
#include "golxzn/core/utils/traits.hpp"
#include "golxzn/core/types.hpp"
#include "golxzn/core/math/point.hpp"

namespace golxzn::core {

template<class T>
struct rect{
	using value_type = T;
	using point_type = point2<value_type>;

	constexpr rect() noexcept = default;
	constexpr rect(const rect &) noexcept = default;
	constexpr rect(rect &&) noexcept = default;
	constexpr rect &operator=(const rect &) noexcept = default;
	constexpr rect &operator=(rect &&) noexcept = default;
	constexpr rect(const value_type x, const value_type y, const value_type w, const value_type h) noexcept
		: x{ x }, y{ y }, width{ w }, height{ h } {}

	constexpr bool empty() const noexcept{ return traits::all_from(T{}, x, y, width, height); };

	constexpr value_type left() const noexcept { return x; }
	constexpr value_type top() const noexcept { return y; }
	constexpr value_type right() const noexcept { return x + width; }
	constexpr value_type bottom() const noexcept { return y + height; }
	constexpr value_type area() const noexcept { return width * height; }
	constexpr point_type center() const noexcept { return { right() / 2, bottom() / 2 }; }

	constexpr bool contains(const value_type px, const value_type py) const noexcept {
		return px >= left() && px <= right() && py >= top() && py <= bottom();
	}
	constexpr bool contains(const point_type &p) const noexcept { return contains(p.x, p.y); }

	constexpr std::optional<rect> intersection(const rect &other) const noexcept {
		const auto left{ std::max(this->left(), other.left()) };
		const auto top{ std::max(this->top(), other.top()) };
		const auto right{ std::min(this->right(), other.right()) };
		const auto bottom{ std::min(this->bottom(), other.bottom()) };
		if (left <= right && top <= bottom) {
			return rect<T>{ left, top, right - left, bottom - top };
		}
		return std::nullopt;
	}

	constexpr bool has_intersection(const rect &other) const noexcept {
		const auto left{ std::max(this->left(), other.left()) };
		const auto right{ std::min(this->right(), other.right()) };
		const auto top{ std::max(this->top(), other.top()) };
		const auto bottom{ std::min(this->bottom(), other.bottom()) };
		return top <= bottom && left <= right;
	}

	constexpr bool operator==(const rect &other) const noexcept {
		return x == other.x && y == other.y && width == other.width && height == other.height;
	}
	constexpr bool operator!=(const rect &other) const noexcept { return !(*this == other); }
	constexpr bool operator<(const rect &other) const noexcept { return area() < other.area(); }
	constexpr bool operator>(const rect &other) const noexcept { return other.area() < area(); }
	constexpr bool operator<=(const rect &other) const noexcept { return !(*this > other); }
	constexpr bool operator>=(const rect &other) const noexcept { return !(*this < other); }

	value_type x{};
	value_type y{};
	value_type width{};
	value_type height{};
};

} // namespace golxzn::core
