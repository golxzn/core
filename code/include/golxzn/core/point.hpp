#pragma once

#include <golxzn/core/export.hpp>
#include <golxzn/core/types.hpp>
#include <golxzn/core/traits.hpp>
#include <cmath>
#include <iomanip>

namespace golxzn::core {

#pragma pack(push, 1)

template<class T, size_t Length>
class GOLXZN_EXPORT Point {
	static_assert(Length != 0, "❌ [golxzn::core::point] Point has to have at least 1 element!");
public:
	static constexpr size_t length() { return Length; }

	Point() = default;

	template<class ...Args,
		std::enable_if_t<traits::are_same_v<T, Args...>, bool> = false>
	Point(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } { }

	[[nodiscard]] constexpr T operator[](const usize index) const {
		if (index < Length) return mValues[index];
		throw std::out_of_range{
			"golxzn::core::Point<T, " + std::to_string(Length) +
			">::operator[] or at() could't take " + std::to_string(index) +
			" because the Length is " + std::to_string(Length)
		};
	}
	[[nodiscard]] T &operator[](const usize index) {
		if (index < Length) return mValues[index];
		throw std::out_of_range{
			"golxzn::core::Point<T, " + std::to_string(Length) +
			">::operator[] or at() could't take " + std::to_string(index) +
			" because the Length is " + std::to_string(Length)
		};
	}

	[[nodiscard]] T at(const usize index) const { return (*this)[index]; }
	[[nodiscard]] T &at(const usize index) { return (*this)[index]; }

	[[nodiscard]] real distanceSquared(const Point &other) const {
		real sum{};
		for(usize i{}; i < Length; ++i) {
			const real subtraction(other.at(i) - at(i));
			sum += subtraction * subtraction;
		}
		return sum;
	}
	[[nodiscard]] real distance(const Point &other) const {
		/// @todo: Think about fast sqrt, but with doubles
		return std::sqrt(distanceSquared(other));
	}

	T *raw() {
		return mValues;
	}
	const T *const raw() const {
		return mValues;
	}

	template<class S, std::enable_if_t<std::is_convertible_v<S, T>, bool> = false>
	operator Point<S, Length>() const {
		Point<S, Length> other;
		for (usize i{}; i < Length; ++i) {
			other.at(i) = static_cast<S>(at(i));
		}
		return other;
	}

	template<size_t Length2>
	Point<T, Length2> as(const std::initializer_list<usize> ids) const {
		assert(ids.size() >= Length2);
		Point<T, Length2> result;
		usize counter{};
		for (const auto id : ids) {
			result.at(counter++) = at(id);
		}
		return result;
	}

	bool operator==(const Point &other) const {
		if (this == &other)
			return true;

		static constexpr auto eps{ std::numeric_limits<T>::epsilon() };
		for (usize i{}; i < Length; ++i) {
			if (std::abs(mValues[i] - other.mValues[i]) > eps) {
				return false;
			}
		}
		return true;
	}
	bool operator!=(const Point &other) const {
		return !(*this == other);
	}

private:
	T mValues[Length]{};
};

template<class T>
class Point<T, 1> {
public:
	static constexpr size_t length() { return 1; }

	constexpr Point() = default;
	constexpr explicit Point(T value) noexcept : value{ value } { }

	[[nodiscard]] constexpr T operator[](const usize index) const {
		if (index == 0) return value;
		throw std::out_of_range{
			"golxzn::core::Point<T, 1>::operator[] or at() could take only one index value which is 0"
		};
	}
	[[nodiscard]] T &operator[](const usize index) {
		if (index == 0) return value;
		throw std::out_of_range{
			"golxzn::core::Point<T, 1>::operator[] or at() could take only one index value which is 0"
		};
	}

	[[nodiscard]] constexpr T at(const usize index) const { return (*this)[index]; }
	[[nodiscard]] T &at(const usize index) { return (*this)[index]; }


	[[nodiscard]] constexpr real distanceSquared(const Point &other) const {
		const real subtraction(other.value - value);
		return subtraction * subtraction;
	}
	[[nodiscard]] constexpr real distance(const Point &other) const {
		const real subtraction(other.value - value);
		return subtraction < 0.0 ? -subtraction : subtraction;
	}

	T *raw() {
		return &value;
	}
	const T *const raw() const {
		return &value;
	}

	template<class S, std::enable_if_t<std::is_convertible_v<S, T>, bool> = false>
	constexpr operator Point<S, 1>() const {
		return Point<S, 1>{ static_cast<S>(value) };
	}

private:
	T value{};
};

#pragma pack(pop)

template<class T> using Point2 = Point<T, 2>;
template<class T> using Point3 = Point<T, 3>;
template<class T> using Point4 = Point<T, 4>;

} // namespace golxzn::core

template<class T, size_t Length>
inline std::ostream &operator<<(std::ostream &out, const golxzn::core::Point<T, Length> &point) {
	if constexpr (Length == 1) {
		return out << "[" << point.value << "]";
	}

	out << '[' << point.at(0);
	for (golxzn::core::usize i{ 1 }; i < Length; ++i) {
		out << ", " << std::setprecision(3) << std::fixed << point.at(i);
	}
	return out << ']';
}

