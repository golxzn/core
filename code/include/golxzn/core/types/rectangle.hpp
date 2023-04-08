#pragma once

#include <optional>
#include <glm/vec2.hpp>
#include "golxzn/core/utils/traits.hpp"
#include "golxzn/core/aliases.hpp"

namespace golxzn::core {

template<class T>
struct rect{
	using value_type = T;
	template<class U> using point_type = glm::vec<2, U>;

	constexpr rect() noexcept = default;
	constexpr rect(const rect &) noexcept = default;
	constexpr rect(rect &&) noexcept = default;
	constexpr rect &operator=(const rect &) noexcept = default;
	constexpr rect &operator=(rect &&) noexcept = default;
	constexpr rect(const value_type x, const value_type y, const value_type w, const value_type h) noexcept
		: x{ x }, y{ y }, width{ w }, height{ h } {}

	template<class U>
	constexpr rect(const rect<U> &other) noexcept
		: x{ static_cast<value_type>(other.x) }, y{ static_cast<value_type>(other.y) }
		, width{ static_cast<value_type>(other.width) }, height{ static_cast<value_type>(other.height) } {}

	constexpr bool empty() const noexcept{ return traits::all_from(T{}, x, y, width, height); };

	constexpr value_type left() const noexcept { return x; }
	constexpr value_type top() const noexcept { return y; }
	constexpr value_type right() const noexcept { return x + width; }
	constexpr value_type bottom() const noexcept { return y + height; }
	constexpr value_type area() const noexcept { return width * height; }
	constexpr point_type<T> center() const noexcept { return { right() / 2, bottom() / 2 }; }

	template<class U>
	constexpr rect &translate(const point_type<U> &offset) noexcept {
		x += static_cast<T>(offset.x);
		y += static_cast<T>(offset.y);
		return *this;
	}
	template<class U>
	constexpr rect translate(const point_type<U> &offset) const noexcept { return rect{ *this }.move(offset); }

	template<class U>
	constexpr bool contains(const U point_x, const U point_y) const noexcept {
		const auto px{ static_cast<T>(point_x) };
		const auto py{ static_cast<T>(point_y) };
		return px >= left() && px <= right() && py >= top() && py <= bottom();
	}
	template<class U>
	constexpr bool contains(const point_type<U> &p) const noexcept { return contains(p.x, p.y); }

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

	constexpr rect overlap(const rect &other) const noexcept {
		const auto left{ std::min(this->left(), other.left()) };
		const auto top{ std::min(this->top(), other.top()) };
		const auto right{ std::max(this->right(), other.right()) };
		const auto bottom{ std::max(this->bottom(), other.bottom()) };
		return rect<T>{ left, top, right - left, bottom - top };
	}

	constexpr bool operator==(const rect &other) const noexcept {
		return x == other.x && y == other.y && width == other.width && height == other.height;
	}
	constexpr bool operator!=(const rect &other) const noexcept { return !(*this == other); }
	constexpr bool operator<(const rect &other) const noexcept { return area() < other.area(); }
	constexpr bool operator>(const rect &other) const noexcept { return other.area() < area(); }
	constexpr bool operator<=(const rect &other) const noexcept { return !(*this > other); }
	constexpr bool operator>=(const rect &other) const noexcept { return !(*this < other); }

	template<class U>
	explicit constexpr operator rect<U>() const noexcept {
		return rect<U>{
			static_cast<U>(x), static_cast<U>(y),
			static_cast<U>(width), static_cast<U>(height)
		};
	}

	value_type x{};
	value_type y{};
	value_type width{};
	value_type height{};
};

} // namespace golxzn::core
