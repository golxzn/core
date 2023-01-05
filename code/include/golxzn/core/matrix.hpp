#pragma once

#include <array>
#include <limits>
#include <exception>
#include <algorithm>

#include <golxzn/core/types.hpp>
#include <golxzn/core/traits.hpp>

namespace golxzn::core {

template<class T, usize Columns, usize Rows>
class Matrix {
	static_assert(Columns != 0 && Rows != 0,
		"golxzn::core::Matrix has to have at least one row and one column");
	static constexpr auto Length{ Columns * Rows };

	template<class T, usize C, usize R> struct submat { using value = Matrix<T, C - 1, R - 1>; };
	template<class T> struct submat<T, 1, 1> { using value = void; };
	template<class T> struct submat<T, 0, 0> { using value = void; };

public:
	static constexpr auto one{ static_cast<T>(1) };
	using submatrix = typename submat<T, Columns, Rows>::value;
	using transpositioned_matrix = Matrix<T, Rows, Columns>;
	using value_type = T;
	using values_container = std::array<value_type, Length>;

	[[nodiscard]] static constexpr usize columns() { return Columns; }
	[[nodiscard]] static constexpr usize rows() { return Rows; }
	[[nodiscard]] static constexpr usize length() { return Length; }
	[[nodiscard]] static constexpr usize size() { return length(); } ///< Alias for length()

	Matrix() = default;

	template<class ...Args, std::enable_if_t<traits::are_same_v<T, Args...>, bool> = false>
	constexpr Matrix(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } {}

	constexpr explicit Matrix(values_container &&values) noexcept : mValues{ std::move(values) } {}

	[[nodiscard]] constexpr T &at(const usize row, const usize column) {
		if (row >= rows() || column >= columns()) {
			throw std::out_of_range{ "..." };
		}
		return at(index(row, column));
	}
	[[nodiscard]] constexpr T at(const usize row, const usize column) const {
		static_assert(row < rows() && column < columns(), "...");
		return mValues[index(row, column)];
	}
	[[nodiscard]] constexpr T at(const usize index) const {
		return mValues.at(index);
	}
	[[nodiscard]] constexpr T &at(const usize index) {
		return mValues.at(index);
	}

	[[nodiscard]] T *data() { return mValues.data(); }
	[[nodiscard]] const T *data() const { return mValues.data(); }

	[[nodiscard]] static constexpr usize index(const usize row, const usize column) {
		return row * columns() + column;
	}
	[[nodiscard]] static constexpr Matrix identity() {
		return Matrix{ identityValues() };
	}
	[[nodiscard]] static constexpr Matrix zero() {
		return Matrix{ values_container{} };
	}
	Matrix &makeIdentity() {
		mValues = identityValues();
		return *this;
	}
	Matrix &reverse() {
		if (rows() != columns()) return *this;

		for (usize row{}; row < rows(); ++row) {
			for (usize column{}; column < columns(); ++column) {
				if (row == column) continue;
				std::swap(at(row, column), at(column, row));
			}
		}
		return *this;
	}
	[[nodiscard]] constexpr transpositioned_matrix transposition() const {
		transpositioned_matrix result;
		for (usize row{}; row < rows(); ++row) {
			for (usize column{ row + 1 }; column < columns(); ++column) {
				std::swap(result[index(row, column)], result[index(column, row)]);
			}
		}
		return result;
	}
	[[nodiscard]] constexpr submatrix subMatrix(const usize row, const usize column) const {
		if constexpr (std::is_void_v<submatrix>) return;
		if (columns() == 1 || rows() == 1) return {};

		submatrix result;
		for (usize _row{}; _row < rows(); ++_row) {
			if (_row == row) continue;
			const usize rowId{ _row > row ? _row - 1 : _row };
			for (usize _column{}; _column < columns(); ++_column) {
				if (_column == column) continue;
				const usize columnId{ _column > column ? _column - 1 : _column };
				result.at(rowId, columnId) = at(_row, _column);
			}
		}
		return result;
	}

	[[nodiscard]] constexpr bool operator==(const Matrix &other) const {
		if (this == &other) return true;

		for (usize index{}; index < length(); ++index) {
			if constexpr (std::is_floating_point_v<T>) {
				if (std::abs(at(index) - other.at(index)) > std::numeric_limits<T>::epsilon()) {
					return false;
				}
			} else {
				if (at(index) != other.at(index)) {
					return false;
				}
			}
		}
		return true;
	}
	[[nodiscard]] constexpr bool operator!=(const Matrix &other) const {
		return !(*this == other);
	}

	[[nodiscard]] Matrix &operator+=(const Matrix &other) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(other.mValues),
			std::begin(mValues), std::plus<int>());
		return *this;
	}
	[[nodiscard]] Matrix &operator-=(const Matrix &other) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(other.mValues),
			std::begin(mValues), std::minus<int>());
		return *this;
	}
	[[nodiscard]] Matrix &operator*=(const T &value) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(mValues),
			[&value](const T &elem) { return elem * value; });
		return *this;
	}
	[[nodiscard]] Matrix &operator/=(const T &value) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(mValues),
			[&value](const T &elem) { return elem / value; });
		return *this;
	}
	[[nodiscard]] constexpr Matrix operator+(const Matrix &other) const {
		return Matrix{ *this } += other;
	}
	[[nodiscard]] constexpr Matrix operator-(const Matrix &other) const {
		return Matrix{ *this } -= other;
	}
	[[nodiscard]] constexpr Matrix operator*(const T &value) const {
		return Matrix{ *this } *= value;
	}
	[[nodiscard]] constexpr Matrix operator/(const T &value) const {
		return Matrix{ *this } /= value;
	}

	[[nodiscard]] constexpr typename values_container::iterator begin() noexcept { return mValues.begin(); }
	[[nodiscard]] constexpr typename values_container::const_iterator begin() const noexcept { return mValues.begin(); }
	[[nodiscard]] constexpr typename values_container::iterator end() noexcept { return mValues.end(); }
	[[nodiscard]] constexpr typename values_container::const_iterator end() const noexcept { return mValues.end(); }
	[[nodiscard]] constexpr typename values_container::reverse_iterator rbegin() noexcept { return mValues.rbegin(); }
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator rbegin() const noexcept { return mValues.rbegin(); }
	[[nodiscard]] constexpr typename values_container::reverse_iterator rend() noexcept { return mValues.rend(); }
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator rend() const noexcept { return mValues.rend(); }
	[[nodiscard]] constexpr typename values_container::const_iterator cbegin() const noexcept { return mValues.cbegin(); }
	[[nodiscard]] constexpr typename values_container::const_iterator cend() const noexcept { return mValues.cend(); }
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator crbegin() const noexcept { return mValues.crbegin(); }
	[[nodiscard]] constexpr typename values_container::const_reverse_iterator crend() const noexcept { return mValues.crend(); }

private:
	values_container mValues{ identityValues() };

	static constexpr values_container identityValues() {
		if constexpr (rows() != columns()) return {};
		else if constexpr (rows() == 1) return { 1 };
		else if constexpr (rows() == 2) return { 1, 0,  0, 1 };
		else if constexpr (rows() == 3) return { 1, 0, 0,  0, 1, 0,  0, 0, 1 };
		else if constexpr (rows() == 4) return { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
		else {
			values_container values{};
			for (usize id{}; id != columns(); ++id) {
				values.at(index(id, id)) = one;
			}
			return values;
		}
	}
};

template<class T>
using mat3 = Matrix<T, 3, 3>;

using mat3sr = mat3<sreal>;
using mat3f = mat3sr;
using mat3r = mat3<real>;
using mat3d = mat3r;
using mat3i32 = mat3<i32>;
using mat3i = mat3i32;

template<class T>
using mat4 = Matrix<T, 4, 4>;

using mat4sr = mat4<sreal>;
using mat4f = mat4sr;
using mat4r = mat4<real>;
using mat4d = mat4r;
using mat4i32 = mat4<i32>;
using mat4i = mat4i32;

} // namespace golxzn::core
