#pragma once

#include <cmath>
#include <iomanip>

#include <golxzn/core/export.hpp>
#include <golxzn/core/types.hpp>
#include <golxzn/core/traits.hpp>

namespace golxzn::core {

#pragma pack(push, 1)

template<class T, size_t Length>
class GOLXZN_EXPORT point {
	static_assert(Length != 0, "❌ [golxzn::core::point] point has to have at least 1 element!");
public:
	static constexpr size_t length() { return Length; }

	constexpr point() = default;

	template<class ...Args, std::enable_if_t<traits::are_same_v<T, Args...>, bool> = false>
	constexpr point(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } { }

	[[nodiscard]] constexpr T operator[](const usize index) const {
		if (index < Length) return mValues[index];
		throw std::out_of_range{ getOutOfRangeError(index) };
	}
	[[nodiscard]] constexpr T &operator[](const usize index) {
		if (index < Length) return mValues[index];
		throw std::out_of_range{ getOutOfRangeError(index) };
	}

	[[nodiscard]] constexpr T at(const usize index) const { return (*this)[index]; }
	[[nodiscard]] constexpr T &at(const usize index) { return (*this)[index]; }

	[[nodiscard]] constexpr f32 distanceSquared(const point &other) const {
		f32 sum{};
		for(usize i{}; i < Length; ++i) {
			const f32 subtraction{ static_cast<f32>(other.at(i) - at(i)) };
			sum += subtraction * subtraction;
		}
		return sum;
	}
	[[nodiscard]] constexpr f32 distance(const point &other) const {
		/// @todo: Think about fast sqrt, but with doubles
		return std::sqrt(distanceSquared(other));
	}

	[[nodiscard]] constexpr T *raw() {
		return mValues;
	}
	[[nodiscard]] constexpr T *const raw() const {
		return mValues;
	}

	template<class S, std::enable_if_t<std::is_convertible_v<S, T>, bool> = false>
	[[nodiscard]] constexpr operator point<S, Length>() const {
		point<S, Length> other;
		for (usize i{}; i < Length; ++i) {
			other.at(i) = static_cast<S>(at(i));
		}
		return other;
	}

	template<size_t Length2>
	[[nodiscard]] constexpr point<T, Length2> as(const std::initializer_list<usize> ids) const {
		assert(ids.size() >= Length2);
		point<T, Length2> result;
		usize counter{};
		for (const auto id : ids) {
			result.at(counter++) = at(id);
		}
		return result;
	}

	[[nodiscard]] constexpr bool operator==(const point &other) const {
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
	[[nodiscard]] constexpr bool operator!=(const point &other) const {
		return !(*this == other);
	}

private:
	T mValues[Length]{};

	static constexpr std::string getOutOfRangeError(const usize index) {
		return "golxzn::core::point<T, " + std::to_string(Length)
			+ ">::operator[] or at() could't take " + std::to_string(index)
			+ " because the Length is " + std::to_string(Length);
	}
};

template<class T>
class point<T, 1> {
public:
	static constexpr size_t length() { return 1; }

	constexpr point() = default;
	constexpr explicit point(T value) noexcept : value{ value } { }

	[[nodiscard]] constexpr T operator[](const usize index) const {
		if (index == 0) return value;
		throw std::out_of_range{
			"golxzn::core::point<T, 1>::operator[] or at() could take only one index value which is 0"
		};
	}
	[[nodiscard]] T &operator[](const usize index) {
		if (index == 0) return value;
		throw std::out_of_range{
			"golxzn::core::point<T, 1>::operator[] or at() could take only one index value which is 0"
		};
	}

	[[nodiscard]] constexpr T at(const usize index) const { return (*this)[index]; }
	[[nodiscard]] T &at(const usize index) { return (*this)[index]; }


	[[nodiscard]] constexpr f32 distanceSquared(const point &other) const {
		const f32 subtraction(other.value - value);
		return subtraction * subtraction;
	}
	[[nodiscard]] constexpr f32 distance(const point &other) const {
		const f32 subtraction(other.value - value);
		return subtraction < 0.0 ? -subtraction : subtraction;
	}

	[[nodiscard]] constexpr T *raw() {
		return &value;
	}
	[[nodiscard]] constexpr T *const raw() const {
		return &value;
	}

	template<class S, std::enable_if_t<std::is_convertible_v<S, T>, bool> = false>
	[[nodiscard]] constexpr operator point<S, 1>() const {
		return point<S, 1>{ static_cast<S>(value) };
	}

private:
	T value{};
};

#pragma pack(pop)

template<class T> using point2 = point<T, 2>;
template<class T> using point3 = point<T, 3>;
template<class T> using point4 = point<T, 4>;

} // namespace golxzn::core

template<class T, size_t Length>
inline std::ostream &operator<<(std::ostream &out, const golxzn::core::point<T, Length> &point) {
	if constexpr (Length == 1) {
		return out << "[" << point.value << "]";
	}

	out << '[' << point.at(0);
	for (golxzn::core::usize i{ 1 }; i < Length; ++i) {
		out << ", " << std::setprecision(3) << std::fixed << point.at(i);
	}
	return out << ']';
}

