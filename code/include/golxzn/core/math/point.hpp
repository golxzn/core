#pragma once

#include <cmath>
#include <array>
#include <string>
#include <iomanip>
#include <execution>
#include <algorithm>

#include <golxzn/core/export.hpp>
#include <golxzn/core/utils/numeric.hpp>

namespace golxzn::core {

#pragma pack(push, 1)

template<class T, usize Count>
class GOLXZN_EXPORT point {
	static_assert(Count >= 1, "❌ [golxzn::core::point] point has to have at least 1 elements!");
public:
	using value_type = std::remove_all_extents_t<T>;
	using values_container = std::array<value_type, Count>;

	static constexpr value_type zero{};
	static constexpr auto one{ static_cast<value_type>(1) };
	[[nodiscard]] static constexpr usize count() noexcept { return Count; }

	constexpr point() noexcept = default;

	constexpr explicit point(values_container &&values) noexcept : mValues{ std::move(values) } {}

	template<class ...Args, std::enable_if_t<traits::are_convertible_v<T, Args...>, bool> = false>
	constexpr explicit point(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } { }

	[[nodiscard]] constexpr value_type operator[](const usize index) const {
		if (index < count()) return mValues[static_cast<size_t>(index)];
		throw std::out_of_range{ getOutOfRangeError(index) };
	}
	[[nodiscard]] constexpr value_type &operator[](const usize index) {
		if (index < count()) return mValues[static_cast<size_t>(index)];
		throw std::out_of_range{ getOutOfRangeError(index) };
	}

	[[nodiscard]] constexpr value_type at(const usize index) const { return (*this)[index]; }
	[[nodiscard]] constexpr value_type &at(const usize index) { return (*this)[index]; }

	[[nodiscard]] constexpr value_type length_no_squared() const noexcept {
		return std::transform_reduce(std::execution::par,
			std::begin(mValues), std::end(mValues), zero, std::plus<>{},
			[](const value_type &value) { return value * value; }
		);
	}

	[[nodiscard]] constexpr value_type length() const noexcept {
		return std::sqrt(length_no_squared());
	}

	[[nodiscard]] constexpr f32 distance_squared(const point &other) const {
		return std::transform_reduce(std::execution::par,
			std::begin(mValues), std::end(mValues), std::begin(other.mValues), zero, std::plus<>{},
			[&](const value_type &value, const value_type &other_value) {
				const f32 subtraction{ static_cast<f32>(other_value - value) };
				return subtraction * subtraction;
			}
		);
	}
	[[nodiscard]] constexpr f32 distance(const point &other) const {
		/// @todo: Think about fast sqrt, but with doubles
		return std::sqrt(distance_squared(other));
	}

	constexpr point &normalize() { return *this /= length(); }
	[[nodiscard]] constexpr point normalize() const { return (*this) / length(); }

	constexpr point &inverse() {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(mValues), [](const value_type &value) { return one / value; }
		);
		return *this;
	}
	[[nodiscard]] constexpr point inverse() const {
		values_container result{};
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(result), [](const value_type &value) { return one / value; }
		);
		return point{ std::move(result) };
	}

	[[nodiscard]] constexpr f32 dot_product(const point &other) const noexcept {
		return std::transform_reduce(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(other.mValues), zero, std::plus<>{},
			[](auto value, auto other_value) { return value * other_value; }
		);
	}

	[[nodiscard]] constexpr value_type *raw() {
		return &mValues[0];
	}
	[[nodiscard]] constexpr value_type *const raw() const {
		return mValues.data();
	}

	template<class S, std::enable_if_t<std::is_convertible_v<S, T>, bool> = false>
	[[nodiscard]] constexpr operator point<S, Count>() const {
		typename point<S, Count>::values_container result;
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(result), [](const auto value) { return static_cast<S>(value); });
		return point<S, Count>{ std::move(result) };
	}

	template<usize Length2>
	[[nodiscard]] constexpr point<T, Length2> as(const std::initializer_list<usize> ids) const {
		point<T, Length2> result;
		usize counter{};

		#pragma omp parallel for
		for (const auto id : ids) {
			result.at(counter++) = at(id);
			if (counter == Length2)
				break;
		}
		return result;
	}

	[[nodiscard]] constexpr bool operator==(const point &other) const {
		if (this == &other)
			return true;

		for (usize i{}; i < count(); ++i) {
			if constexpr (std::is_floating_point_v<T>) {
				if (!utils::numeric::approximately::are_equal(at(i), other.at(i))) {
					return false;
				}
			} else {
				if (at(i) != other.at(i)) {
					return false;
				}
			}
		}
		return true;
	}
	[[nodiscard]] constexpr bool operator!=(const point &other) const {
		return !(*this == other);
	}

	constexpr point &operator*=(const value_type other) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(mValues), [&other](const auto value) { return value * other; });

		return *this;
	}
	constexpr point &operator/=(const value_type other) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(mValues), [&other](const auto value) { return value / other; });
		return *this;
	}

	[[nodiscard]] constexpr point operator*(const value_type other) const {
		return point{ *this } *= other;
	}
	[[nodiscard]] constexpr point operator/(const value_type other) const {
		return point{ *this } /= other;
	}

	[[nodiscard]] constexpr point operator-() const noexcept {
		return point{ *this } *= -1;
	}

	template<class X, usize XLength>
	[[nodiscard]] friend constexpr point operator*(const typename point<X, XLength>::value_type other, const point<X, XLength> &point) {
		return point * other;
	}
	template<class X, usize XLength>
	[[nodiscard]] friend constexpr point operator/(const typename point<X, XLength>::value_type other, const point<X, XLength> &point) {
		return point / other;
	}

	[[nodiscard]] constexpr typename values_container::iterator               begin()   noexcept       { return mValues.begin();  }
	[[nodiscard]] constexpr typename values_container::const_iterator         begin()   const noexcept { return mValues.begin();  }
	[[nodiscard]] constexpr typename values_container::iterator               end()     noexcept       { return mValues.end();    }
	[[nodiscard]] constexpr typename values_container::const_iterator         end()     const noexcept { return mValues.end();    }
	[[nodiscard]] constexpr typename values_container::reverse_iterator       rbegin()  noexcept       { return mValues.rbegin(); }
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator rbegin()  const noexcept { return mValues.rbegin(); }
	[[nodiscard]] constexpr typename values_container::reverse_iterator       rend()    noexcept       { return mValues.rend();   }
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator rend()    const noexcept { return mValues.rend();   }
	[[nodiscard]] constexpr typename values_container::const_iterator         cbegin()  const noexcept { return mValues.cbegin(); }
	[[nodiscard]] constexpr typename values_container::const_iterator         cend()    const noexcept { return mValues.cend();   }
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator crbegin() const noexcept { return mValues.crbegin();}
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator crend()   const noexcept { return mValues.crend();  }

private:
	values_container mValues{};

	static constexpr std::string getOutOfRangeError(const usize index) {
		return "golxzn::core::point<T, " + std::to_string(count())
			+ ">::operator[] or at() could't take " + std::to_string(index)
			+ " because the Count is " + std::to_string(count());
	}
};

#pragma pack(pop)

template<class T> using point2 = point<T, 2>;

using point2f32 = point2<f32>;
using point2f = point2f32;
using point2f64 = point2<f64>;
using point2d = point2f64;
using point2i32 = point2<i32>;
using point2i = point2i32;
using point2u32 = point2<u32>;
using point2u = point2u32;

template<class T> using point3 = point<T, 3>;

using point3f32 = point3<f32>;
using point3f = point3f32;
using point3f64 = point3<f64>;
using point3d = point3f64;
using point3i32 = point3<i32>;
using point3i = point3i32;
using point3u32 = point3<u32>;
using point3u = point3u32;

template<class T> using point4 = point<T, 4>;

using point4f32 = point4<f32>;
using point4f = point4f32;
using point4f64 = point4<f64>;
using point4d = point4f64;
using point4i32 = point4<i32>;
using point4i = point4i32;
using point4u32 = point4<u32>;
using point4u = point4u32;

enum class axis : u8 { X = 'x', Y = 'y', Z = 'z' };

template<class T, usize Count>
constexpr point<T, Count> get_axis(const axis dir) noexcept {
	using result_type = point<T, Count>;
	result_type result;
	switch (dir) {
		case axis::X:
		 	result.at(0) = result_type::one;
			break;
		case axis::Y:
			result.at(1) = result_type::one;
			break;
		case axis::Z:
			if constexpr (result_type::count() > 2) {
				result.at(2) = result_type::one;
			}
		default: break;
	}
	return result;
}

template<class T, usize Count, axis Dir = axis::Y>
constexpr point<T, Count> get_axis() noexcept {
	using result_type = point<T, Count>;

	if constexpr (Dir == axis::X) {
		if constexpr (result_type::count() == 2) {
			return result_type{ result_type::one, T{} };
		} else if (result_type::count() == 3) {
			return result_type{ result_type::one, T{}, T{} };
		}
		result_type result{};
		result.at(0) = result_type::one;
		return result;
	} else if constexpr (Dir == axis::Y) {
		if constexpr (result_type::count() == 2) {
			return result_type{ T{}, result_type::one };
		} else if (result_type::count() == 3) {
			return result_type{ T{}, result_type::one, T{} };
		}
		result_type result{};
		result.at(1) = result_type::one;
		return result;
	} else if constexpr (Dir == axis::Z) {
		if constexpr (result_type::count() == 2) {
			return result_type{};
		} else if (result_type::count() == 3) {
			return result_type{ T{}, T{}, result_type::one };
		}
		result_type result{};
		result.at(2) = result_type::one;
		return result;
	}
}


} // namespace golxzn::core

template<class T, golxzn::core::usize Count>
inline std::ostream &operator<<(std::ostream &out, const golxzn::core::point<T, Count> &point) {
	if constexpr (Count == 1) {
		return out << "[" << point.value << "]";
	}

	out << '[' << point.at(0);
	for (golxzn::core::usize i{ 1 }; i < point.count(); ++i) {
		out << ", " << std::setprecision(3) << std::fixed << point.at(i);
	}
	return out << ']';
}

