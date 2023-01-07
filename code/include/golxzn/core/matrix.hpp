#pragma once

#include <iomanip>
#include <array>
#include <limits>
#include <exception>
#include <algorithm>

#include <golxzn/core/export.hpp>
#include <golxzn/core/types.hpp>
#include <golxzn/core/traits.hpp>

namespace golxzn::core {

template<class T, usize Rows, usize Columns>
class GOLXZN_EXPORT mat {
	static_assert(Columns != 0 && Rows != 0,
		"❌ [golxzn::core::mat] has to have at least one row and one column");
	static constexpr auto Length{ Columns * Rows };

	template<class T, usize C, usize R> struct _submat { using value = mat<T, C - 1, R - 1>; };
	template<class T> struct _submat<T, 1, 1> { using value = void; };
	template<class T> struct _submat<T, 0, 0> { using value = void; };

public:
	using submat = typename _submat<T, Rows, Columns>::value;
	using transpositioned_matrix = mat<T, Columns, Rows>;
	using value_type = T;
	using values_container = std::array<value_type, Length>;
	using size_type = typename values_container::size_type;

	static constexpr auto one{ static_cast<value_type>(1) };
	[[nodiscard]] static constexpr usize columns() { return Columns; }
	[[nodiscard]] static constexpr usize rows() { return Rows; }
	[[nodiscard]] static constexpr usize length() { return Length; }
	[[nodiscard]] static constexpr usize size() { return length(); } ///< Alias for length()

	mat() = default;

	template<class ...Args, std::enable_if_t<traits::are_same_v<T, Args...>, bool> = false>
	constexpr mat(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } {}

	constexpr explicit mat(values_container &&values) noexcept : mValues{ std::move(values) } {}

	constexpr mat(const mat &other) noexcept : mValues{ other.mValues } {}
	constexpr mat(mat &&other) noexcept : mValues{ std::move(other.mValues) } {}

	[[nodiscard]] constexpr T &at(const usize row, const usize column) {
		if (row >= rows() || column >= columns()) {
			throw std::out_of_range{ getOutOfRangeError(row, column) };
		}
		return at(index(row, column));
	}
	[[nodiscard]] constexpr T at(const usize row, const usize column) const {
		if (row >= rows() || column >= columns()) {
			throw std::out_of_range{ getOutOfRangeError(row, column) };
		}
		return mValues[static_cast<size_type>(index(row, column))];
	}
	[[nodiscard]] constexpr T at(const usize index) const {
		return mValues.at(static_cast<size_type>(index));
	}
	[[nodiscard]] constexpr T &at(const usize index) {
		return mValues.at(static_cast<size_type>(index));
	}

	[[nodiscard]] constexpr T determinant() const {
		static_assert(columns() == rows(), "Matrix must be square");
		if constexpr (columns() == 1) {
			return mValues.at(0);
		} else if constexpr (columns() == 2) {
			return mValues.at(0) * mValues.at(3) - mValues.at(1) * mValues.at(2);
		}

		T det{};
		for (size_type i = 0; i < columns(); ++i) {
			det += mValues.at(i) * subMatrix(0, i).determinant() * ((i % 2 == 0) ? 1 : -1);
		}
		return det;
	}

	[[nodiscard]] T *data() { return mValues.data(); }
	[[nodiscard]] const T *data() const { return mValues.data(); }

	[[nodiscard]] static constexpr usize index(const usize row, const usize column) {
		return row * columns() + column;
	}
	[[nodiscard]] static constexpr mat identity() {
		return mat{ identityValues() };
	}
	[[nodiscard]] static constexpr mat zero() {
		return mat{ values_container{} };
	}
	constexpr mat &makeIdentity() {
		mValues = identityValues();
		return *this;
	}
	constexpr mat &makeZero() {
		std::fill_n(mValues, mValues.size(), static_cast<T>(0));
		return *this;
	}

	constexpr mat &reverse() {
		if (rows() != columns()) {
			throw std::runtime_error{ "Cannot reverse cuz of rows != columns" };
		}

		#pragma omp parallel for
		for (usize row{}; row < rows(); ++row) {
			for (usize column{ row + 1 }; column < columns(); ++column) {
				std::swap(at(row, column), at(column, row));
			}
		}
		return *this;
	}

	mat reverse() const {
		if (rows() != columns()) {
			throw std::runtime_error{ "Cannot reverse cuz of rows != columns" };
		}
		constexpr size_type columnsCount{ static_cast<size_type>(columns()) };

		values_container container{};
		size_type insertPos{};
		for(size_type column{}; column != columnsCount; ++column) {
			for(size_type i{ column }; i < mValues.size(); i += columnsCount) {
				container.at(insertPos++) = mValues.at(i);
			}
		}
		return mat{ std::move(container) };
	}

	[[nodiscard]] constexpr transpositioned_matrix transposition() const {
		constexpr size_type columnsCount{ static_cast<size_type>(columns()) };
		typename transpositioned_matrix::values_container result{};

		size_type insertPos{};
		for(size_type column{}; column != columnsCount; ++column) {
			for(size_type i{ column }; i < mValues.size(); i += columnsCount) {
				result.at(insertPos++) = mValues.at(i);
			}
		}

		return transpositioned_matrix{ std::move(result) };
	}

	[[nodiscard]] constexpr submat subMatrix(const usize row, const usize column) const {
		if constexpr (std::is_void_v<submat>) return;
		if constexpr (columns() == 1 || rows() == 1) return;
		if (columns() == 1 || rows() == 1) return {};

		submat result;
		#pragma omp parallel for
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

	[[nodiscard]] constexpr bool operator==(const mat &other) const {
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
	[[nodiscard]] constexpr bool operator!=(const mat &other) const {
		return !(*this == other);
	}

	constexpr mat &operator+=(const mat &other) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(other.mValues),
			std::begin(mValues), std::plus<int>());
		return *this;
	}
	constexpr mat &operator-=(const mat &other) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(other.mValues),
			std::begin(mValues), std::minus<int>());
		return *this;
	}
	constexpr mat &operator*=(const T &value) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(mValues),
			[&value](const T &elem) { return elem * value; });
		return *this;
	}
	constexpr mat &operator/=(const T &value) {
		std::transform(std::begin(mValues), std::end(mValues), std::begin(mValues),
			[&value](const T &elem) { return elem / value; });
		return *this;
	}
	[[nodiscard]] constexpr mat operator+(const mat &other) const {
		return mat{ *this } += other;
	}
	[[nodiscard]] constexpr mat operator-(const mat &other) const {
		return mat{ *this } -= other;
	}
	[[nodiscard]] constexpr mat operator*(const T &value) const {
		return mat{ *this } *= value;
	}
	[[nodiscard]] constexpr mat operator/(const T &value) const {
		return mat{ *this } /= value;
	}

	template<size_t OtherColumns>
	[[nodiscard]] constexpr mat<T, OtherColumns, Rows> operator*(const mat<T, Columns, OtherColumns> &other) const {
		using result_matrix = mat<T, OtherColumns, Rows>;
		typename result_matrix::values_container result{};

		#pragma omp parallel for
		for (usize mRow{}; mRow < rows(); ++mRow) {
			for (usize oColumn{}; oColumn < other.columns(); ++oColumn) {
				for (usize oRow{}; oRow < other.rows(); ++oRow) {
					result.at(static_cast<size_type>(result_matrix::index(mRow, oColumn)))
						+= at(mRow, oRow) * other.at(oRow, oColumn);
				}
			}
		}

		return result_matrix{ std::move(result) };
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
	values_container mValues{ identityValues() };

	static constexpr values_container identityValues() {
		if constexpr (rows() != columns()) return {};
		else if constexpr (rows() == 1) return { 1 };
		else if constexpr (rows() == 2) return { 1, 0,  0, 1 };
		else if constexpr (rows() == 3) return { 1, 0, 0,  0, 1, 0,  0, 0, 1 };
		else if constexpr (rows() == 4) return { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
		else {
			values_container values{};
			#pragma omp parallel for
			for (size_type id{}; id < length(); id += static_cast<size_type>(columns() + 1)) {
				values[id] = one;
			}
			return values;
		}
	}

	static constexpr std::string getOutOfRangeError(const usize row, const usize column) {
		return "golxzn::core::mat<T, " + std::to_string(columns()) + ", "
			+ std::to_string(rows()) + ">::at() could't take column " + std::to_string(column)
			+ ", and row " + std::to_string(row);
	}
};

template<class T, usize Columns, usize Rows>
using matrix = mat<T, Columns, Rows>;

template<class T, usize Row>
using smat = mat<T, Row, Row>;

template<class T, usize Row>
using square_mat = mat<T, Row, Row>;

template<class T>
using mat2 = smat<T, 2>;

template<class T>
using mat3 = smat<T, 3>;

using mat3sr = mat3<sreal>;
using mat3f = mat3sr;
using mat3r = mat3<real>;
using mat3d = mat3r;
using mat3i32 = mat3<i32>;
using mat3i = mat3i32;

template<class T>
using mat4 = smat<T, 4>;

using mat4sr = mat4<sreal>;
using mat4f = mat4sr;
using mat4r = mat4<real>;
using mat4d = mat4r;
using mat4i32 = mat4<i32>;
using mat4i = mat4i32;

} // namespace golxzn::core

template<class T, size_t Rows, size_t Columns>
inline std::ostream &operator<<(std::ostream &out, const golxzn::core::mat<T, Rows, Columns> &matrix) {
	constexpr std::streamsize paddings{ 4 };

	size_t counter{ 1 };
	for (const auto &value : matrix) {
		if (counter == 1) {
			out << '|';
		}
		out << std::setw(paddings) << " " << std::setprecision(3) << std::fixed << value
			<< std::setw(paddings + 1) << " ";
		if (counter == Columns) {
			out << "|\n";
			counter = 0;
		}
		++counter;
	}
	return out;
}


