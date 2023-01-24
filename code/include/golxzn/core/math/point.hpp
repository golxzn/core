#pragma once

#include <cmath>
#include <iomanip>

#include <golxzn/core/export.hpp>
#include <golxzn/core/types.hpp>
#include <golxzn/core/utils/traits.hpp>

namespace golxzn::core {

#pragma pack(push, 1)

template<class T, usize Length>
class GOLXZN_EXPORT point {
	static_assert(Length >= 2, "❌ [golxzn::core::point] point has to have at least 2 elements!");
public:
	using value_type = std::remove_all_extents_t<T>;

	[[nodiscard]] static constexpr usize length() noexcept { return Length; }
	static constexpr value_type zero{};
	static constexpr auto one{ static_cast<value_type>(1) };

	constexpr point() noexcept = default;

	template<class ...Args, std::enable_if_t<traits::are_same_v<T, Args...>, bool> = false>
	constexpr explicit point(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } { }

	[[nodiscard]] constexpr value_type operator[](const usize index) const {
		if (index < Length) return mValues[index];
		throw std::out_of_range{ getOutOfRangeError(index) };
	}
	[[nodiscard]] constexpr value_type &operator[](const usize index) {
		if (index < Length) return mValues[index];
		throw std::out_of_range{ getOutOfRangeError(index) };
	}

	[[nodiscard]] constexpr value_type at(const usize index) const { return (*this)[index]; }
	[[nodiscard]] constexpr value_type &at(const usize index) { return (*this)[index]; }

	[[nodiscard]] constexpr f32 distanceSquared(const point &other) const {
		f32 sum{};
		#pragma omp parallel for
		for(usize i{}; i < length(); ++i) {
			const f32 subtraction{ static_cast<f32>(other.at(i) - at(i)) };
			sum += subtraction * subtraction;
		}
		return sum;
	}
	[[nodiscard]] constexpr f32 distance(const point &other) const {
		/// @todo: Think about fast sqrt, but with doubles
		return std::sqrt(distanceSquared(other));
	}

	[[nodiscard]] constexpr value_type *raw() {
		return mValues;
	}
	[[nodiscard]] constexpr value_type *const raw() const {
		return mValues;
	}

	template<class S, std::enable_if_t<std::is_convertible_v<S, T>, bool> = false>
	[[nodiscard]] constexpr operator point<S, Length>() const {
		point<S, Length> other;
		#pragma omp parallel for
		for (usize i{}; i < Length; ++i) {
			other.at(i) = static_cast<S>(at(i));
		}
		return other;
	}

	template<usize Length2>
	[[nodiscard]] constexpr point<T, Length2> as(const std::initializer_list<usize> ids) const {
		assert(ids.size() >= Length2);
		point<T, Length2> result;
		usize counter{};
		#pragma omp parallel for
		for (const auto id : ids) {
			result.at(counter++) = at(id);
		}
		return result;
	}

	[[nodiscard]] constexpr bool operator==(const point &other) const {
		if (this == &other)
			return true;

		static constexpr auto eps{ std::numeric_limits<T>::epsilon() };
		#pragma omp parallel for
		for (usize i{}; i < Length; ++i) {
			if (std::abs(mValues[i] - other.mValues[i]) > eps) {
				return false;
			}
		}
		return true;
	}
	[[nodiscard]] constexpr bool operator!=(const point &other) const {
		return !(*this == other);
	}

private:
	value_type mValues[Length]{};

	static constexpr std::string getOutOfRangeError(const usize index) {
		return "golxzn::core::point<T, " + std::to_string(Length)
			+ ">::operator[] or at() could't take " + std::to_string(index)
			+ " because the Length is " + std::to_string(Length);
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

template<class T> using point3 = point<T, 3>;

using point3f32 = point3<f32>;
using point3f = point3f32;
using point3f64 = point3<f64>;
using point3d = point3f64;
using point3i32 = point3<i32>;
using point3i = point3i32;

template<class T> using point4 = point<T, 4>;

using point4f32 = point4<f32>;
using point4f = point4f32;
using point4f64 = point4<f64>;
using point4d = point4f64;
using point4i32 = point4<i32>;
using point4i = point4i32;

enum class Direction : u8 { X = 'x', Y = 'y', Z = 'z' };

template<class T, usize Length>
constexpr point<T, Length> getAxis(const Direction dir) noexcept {
	using result_type = point<T, Length>;
	result_type result;
	switch (dir) {
		case Direction::X:
		 	result.at(0) = result_type::one;
			break;
		case Direction::Y:
			result.at(1) = result_type::one;
			break;
		case Direction::Z:
			if constexpr (result_type::length() > 2) {
				result.at(2) = result_type::one;
			}
		default: break;
	}
	return result;
}

template<class T, usize Length, Direction Dir = Direction::Y>
constexpr point<T, Length> getAxis() noexcept {
	using result_type = point<T, Length>;

	if constexpr (Dir == Direction::X) {
		if constexpr (result_type::length() == 2) {
			return result_type{ result_type::one, T{} };
		} else if (result_type::length() == 3) {
			return result_type{ result_type::one, T{}, T{} };
		}
		result_type result{};
		result.at(0) = result_type::one;
		return result;
	} else if constexpr (Dir == Direction::Y) {
		if constexpr (result_type::length() == 2) {
			return result_type{ T{}, result_type::one };
		} else if (result_type::length() == 3) {
			return result_type{ T{}, result_type::one, T{} };
		}
		result_type result{};
		result.at(1) = result_type::one;
		return result;
	} else if constexpr (Dir == Direction::Z) {
		if constexpr (result_type::length() == 2) {
			return result_type{};
		} else if (result_type::length() == 3) {
			return result_type{ T{}, T{}, result_type::one };
		}
		result_type result{};
		result.at(2) = result_type::one;
		return result;
	}
	return result_type{};
}



} // namespace golxzn::core

template<class T, golxzn::core::usize Length>
inline std::ostream &operator<<(std::ostream &out, const golxzn::core::point<T, Length> &point) {
	if constexpr (Length == 1) {
		return out << "[" << point.value << "]";
	}

	out << '[' << point.at(0);
	for (golxzn::core::usize i{ 1 }; i < point.length(); ++i) {
		out << ", " << std::setprecision(3) << std::fixed << point.at(i);
	}
	return out << ']';
}

