#pragma once

#include <array>
#include <iomanip>
#include <optional>
#include <algorithm>
#include <execution>

#include <golxzn/core/export.hpp>
#include <golxzn/core/math/vector.hpp>
#include <golxzn/core/utils/error.hpp>
#include <golxzn/core/utils/numeric.hpp>

namespace golxzn::core {

template<class T, usize Rows, usize Columns>
class GOLXZN_EXPORT mat {
	static_assert(Columns != 0 && Rows != 0,
		"❌ [golxzn::core::mat] has to have at least one row and one column");
	static constexpr auto Length{ Columns * Rows };

	using submat_error_value = void;
	template<class T, usize C, usize R> struct _submat { using value = mat<T, C - 1, R - 1>; };
	template<class T> struct _submat<T, 1, 1> { using value = submat_error_value; };
	template<class T> struct _submat<T, 0, 0> { using value = submat_error_value; };

public:
	using submat_type = typename _submat<T, Rows, Columns>::value;
	using inverse_matrix = mat<f32, Rows, Columns>;
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
	constexpr explicit mat(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } {}

	constexpr explicit mat(values_container &&values) noexcept : mValues{ std::move(values) } {}

	constexpr mat(const mat &other) noexcept = default;
	constexpr mat(mat &&other) noexcept
		: mValues{ std::move(other.mValues) }, mDeterminant{ std::move(other.mDeterminant) } {}

	constexpr mat &operator=(const mat &other) noexcept {
		if (this == &other) return *this;
		mValues = other.mValues;
		mDeterminant = other.mDeterminant;
		return *this;
	}
	constexpr mat &operator=(mat &&other) noexcept {
		if (this == &other) return *this;
		mValues = std::move(other.mValues);
		mDeterminant = std::move(other.mDeterminant);
		return *this;
	}

	[[nodiscard]] constexpr T &at(const usize row, const usize column) {
		if (row >= rows() || column >= columns()) {
			utils::error::out_of_range(getOutOfRangeError(row, column));
		}
		mDeterminant = std::nullopt;
		return at(index(row, column));
	}
	[[nodiscard]] constexpr T at(const usize row, const usize column) const {
		if (row >= rows() || column >= columns()) {
			utils::error::out_of_range(getOutOfRangeError(row, column));
		}
		return mValues[static_cast<size_type>(index(row, column))];
	}
	[[nodiscard]] constexpr T at(const usize index) const {
		return mValues.at(static_cast<size_type>(index));
	}
	[[nodiscard]] constexpr T &at(const usize index) {
		mDeterminant = std::nullopt;
		return mValues.at(static_cast<size_type>(index));
	}
	[[nodiscard]] constexpr T &operator()(const usize _row, const usize _column) { return at(_row, _column); }
	[[nodiscard]] constexpr T operator()(const usize _row, const usize _column) const { return at(_row, _column); }
	[[nodiscard]] constexpr T &operator()(const usize index) { return at(index); }
	[[nodiscard]] constexpr T operator()(const usize index) const { return at(index); }

	enum class Target{ Rows, Columns };
	constexpr mat &swap(const usize fromId, const usize toId, const Target target = Target::Rows) {
		if (fromId == toId) return *this;

		if (target == Target::Rows) {
			for (usize column{}; column < columns(); ++column) {
				std::swap(at(fromId, column), at(toId, column));
			}
		} else {
			for (usize row{}; row < rows(); ++row) {
				std::swap(at(row, fromId), at(row, toId));
			}
		}
		return *this;
	}

	[[nodiscard]] constexpr T determinant() const { return 0; }
	[[nodiscard]] constexpr bool is_invertible() const { return false; }

	[[nodiscard]] constexpr T *data() {
		mDeterminant = std::nullopt;
		return mValues.data();
	}
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

	constexpr std::optional<inverse_matrix> inverse() const {
		return std::nullopt;
	}

	constexpr mat &make_identity() {
		mDeterminant = std::nullopt;
		mValues = identityValues();
		return *this;
	}
	constexpr mat &make_zero() {
		mDeterminant = std::nullopt;
		std::fill_n(std::execution::par, std::begin(mValues), mValues.size(), static_cast<T>(0));
		return *this;
	}

	constexpr mat &reverse() {
		utils::error::runtime_error("Cannot reverse because the count of rows isn't equal columns");
		return *this;
	}

	mat reverse() const {
		utils::error::runtime_error("Cannot reverse because the count of rows isn't equal columns");
		return mat{};
	}

	[[nodiscard]] constexpr transpositioned_matrix transposition() const {
		constexpr size_type columnsCount{ static_cast<size_type>(columns()) };
		typename transpositioned_matrix::values_container result{};

		size_type insertPos{};
		#pragma omp parallel for
		for(size_type column{}; column != columnsCount; ++column) {
			for(size_type i{ column }; i < mValues.size(); i += columnsCount) {
				result.at(insertPos++) = mValues.at(i);
			}
		}

		return transpositioned_matrix{ std::move(result) };
	}

	[[nodiscard]] constexpr submat_type submat(const usize row, const usize column) const {
		if constexpr (std::is_same_v<submat_type, submat_error_value>) return;
		if constexpr (columns() == 1 || rows() == 1) return;

		submat_type result;
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

	template<usize OtherRows, usize OtherColumns>
	[[nodiscard]] constexpr bool operator==(const mat<T, OtherRows, OtherColumns> &other) const {
		if constexpr (OtherColumns != columns() || OtherRows != rows()) return false;
		if (this == &other) return true;

		for (usize index{}; index < length(); ++index) {
			if constexpr (std::is_floating_point_v<T>) {
				if (!utils::numeric::approximately::are_equal(at(index), other.at(index))) {
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

	template<usize OtherColumns, usize OtherRows>
	[[nodiscard]] constexpr bool operator!=(const mat<T, OtherColumns, OtherRows> &other) const {
		if constexpr (OtherColumns != columns() || OtherRows != rows()) return true;
		return !(*this == other);
	}

	constexpr mat &operator+=(const mat &other) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(other.mValues), std::begin(mValues), std::plus<int>());
		mDeterminant = std::nullopt;
		return *this;
	}
	constexpr mat &operator-=(const mat &other) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(other.mValues), std::begin(mValues), std::minus<int>());
		mDeterminant = std::nullopt;
		return *this;
	}
	constexpr mat &operator*=(const T &value) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(mValues), [&value](const T &elem) { return elem * value; });
		mDeterminant = std::nullopt;
		return *this;
	}
	constexpr mat &operator/=(const T &value) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(mValues), [&value](const T &elem) { return elem / value; });
		mDeterminant = std::nullopt;
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

	constexpr mat &operator*=(const mat &other) {
		return *this = (*this) * other;
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

	[[nodiscard]] constexpr vec<T, Rows> operator*(const vec<T, Rows> &other) const {
		vec<T, Rows> result{};

		#pragma omp parallel for
		for (usize row{}; row < rows(); ++row) {
			for (usize column{}; column < columns(); ++column) {
				result.at(static_cast<size_type>(row)) += at(row, column) * other.at(column);
			}
		}

		return result;
	}

	[[nodiscard]] constexpr mat operator-() const {
		return mat{ *this } *= -1;
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
	mutable std::optional<T> mDeterminant{};
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

template<class T, usize Dimension>
class GOLXZN_EXPORT mat<T, Dimension, Dimension> {
	static_assert(Dimension != 0,
		"❌ [golxzn::core::mat] has to have at least one row and one column");
	static constexpr auto Length{ Dimension * Dimension };

	using submat_error_value = void;
	template<class T, usize C, usize R> struct _submat { using value = mat<T, C - 1, R - 1>; };
	template<class T> struct _submat<T, 1, 1> { using value = submat_error_value; };
	template<class T> struct _submat<T, 0, 0> { using value = submat_error_value; };

public:
	template<class ValueType>
	using separated_matrix = mat<ValueType, Dimension, Dimension * 2>;
	template<class ValueType>
	using separate_matrix_param = mat<ValueType, Dimension, Dimension>;

	using submat_type = typename _submat<T, Dimension, Dimension>::value;
	using inverse_matrix = mat<f32, Dimension, Dimension>;
	using transpositioned_matrix = mat<T, Dimension, Dimension>;
	using value_type = T;
	using values_container = std::array<value_type, Length>;
	using size_type = typename values_container::size_type;

	static constexpr auto one{ static_cast<value_type>(1) };
	[[nodiscard]] static constexpr usize columns() { return Dimension; }
	[[nodiscard]] static constexpr usize rows() { return Dimension; }
	[[nodiscard]] static constexpr usize length() { return Length; }
	[[nodiscard]] static constexpr usize size() { return length(); } ///< Alias for length()

	mat() = default;

	template<class ...Args, std::enable_if_t<traits::are_same_v<T, Args...>, bool> = false>
	constexpr mat(Args ...args) noexcept : mValues{ std::forward<Args>(args)... } {
		if constexpr (columns() <= 2)
			[[maybe_unused]] const auto det{ determinant() };
	}

	constexpr explicit mat(values_container &&values) noexcept : mValues{ std::move(values) } {
		if constexpr (columns() <= 2)
			[[maybe_unused]] const auto det{ determinant() };
	}

	constexpr mat(const mat &other) noexcept
		: mValues{ other.mValues }, mDeterminant{ other.mDeterminant } {}
	constexpr mat(mat &&other) noexcept
		: mValues{ std::move(other.mValues) }, mDeterminant{ std::move(other.mDeterminant) } {}

	constexpr mat &operator=(const mat &other) noexcept {
		if (this == &other) return *this;
		mValues = other.mValues;
		mDeterminant = other.mDeterminant;
		return *this;
	}
	constexpr mat &operator=(mat &&other) noexcept {
		if (this == &other) return *this;
		mValues = std::move(other.mValues);
		mDeterminant = std::move(other.mDeterminant);
		return *this;
	}

	[[nodiscard]] constexpr T &at(const usize row, const usize column) {
		if (row >= rows() || column >= columns()) {
			utils::error::out_of_range(getOutOfRangeError(row, column));
		}
		mDeterminant = std::nullopt;
		return at(index(row, column));
	}
	[[nodiscard]] constexpr T at(const usize row, const usize column) const {
		if (row >= rows() || column >= columns()) {
			utils::error::out_of_range(getOutOfRangeError(row, column));
		}
		return mValues[static_cast<size_type>(index(row, column))];
	}
	[[nodiscard]] constexpr T at(const usize index) const {
		return mValues.at(static_cast<size_type>(index));
	}
	[[nodiscard]] constexpr T &at(const usize index) {
		mDeterminant = std::nullopt;
		return mValues.at(static_cast<size_type>(index));
	}
	[[nodiscard]] constexpr T &operator()(const usize _row, const usize _column) { return at(_row, _column); }
	[[nodiscard]] constexpr T operator()(const usize _row, const usize _column) const { return at(_row, _column); }
	[[nodiscard]] constexpr T &operator()(const usize index) { return at(index); }
	[[nodiscard]] constexpr T operator()(const usize index) const { return at(index); }

	enum class Target{ Rows, Columns };
	constexpr mat &swap(const usize fromId, const usize toId, const Target target = Target::Rows) {
		if (fromId == toId) return *this;

		if (target == Target::Rows) {
			for (usize column{}; column < columns(); ++column) {
				std::swap(at(fromId, column), at(toId, column));
			}
		} else {
			for (usize row{}; row < rows(); ++row) {
				std::swap(at(row, fromId), at(row, toId));
			}
		}
		return *this;
	}

	[[nodiscard]] constexpr T determinant() const {
		if (mDeterminant.has_value()) {
			return mDeterminant.value();
		}
		if constexpr (columns() == 1) {
			mDeterminant = std::make_optional(mValues.at(0));
			return mDeterminant.value();
		} else if constexpr (columns() == 2) {
			mDeterminant = std::make_optional(
				mValues.at(0) * mValues.at(3) - mValues.at(1) * mValues.at(2));
			return mDeterminant.value();
		}

		T det{};
		static constexpr auto sign{ [](const size_type id) { return ((id % 2 == 0) ? 1 : -1); } };
		#pragma omp parallel for
		for (size_type i = 0; i < columns(); ++i) {
			if constexpr (!std::is_same_v<submat_type, submat_error_value>) {
				det += mValues.at(i) * submat(0, i).determinant() * sign(i);
			}
		}
		mDeterminant = std::make_optional(det);
		return det;
	}

	[[nodiscard]] constexpr bool is_invertible() const {
		return determinant() != 0;
	}

	[[nodiscard]] constexpr T *data() {
		mDeterminant = std::nullopt;
		return mValues.data();
	}
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

	constexpr std::optional<inverse_matrix> inverse() const {
		if (!is_invertible()) return std::nullopt;

		inverse_matrix result{};
		return result;
	}

	constexpr mat &make_identity() {
		mDeterminant = std::nullopt;
		mValues = identityValues();
		return *this;
	}
	constexpr mat &make_zero() {
		mDeterminant = std::nullopt;
		std::fill_n(std::execution::par, std::begin(mValues), mValues.size(), static_cast<T>(0));
		return *this;
	}

	constexpr mat &reverse() {
		mDeterminant = std::nullopt;
		#pragma omp parallel for
		for (usize row{}; row < rows(); ++row) {
			for (usize column{ row + 1 }; column < columns(); ++column) {
				std::swap(at(row, column), at(column, row));
			}
		}
		return *this;
	}

	mat reverse() const {
		constexpr size_type columnsCount{ static_cast<size_type>(columns()) };

		values_container container{};
		size_type insertPos{};
		#pragma omp parallel for
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
		#pragma omp parallel for
		for(size_type column{}; column != columnsCount; ++column) {
			for(size_type i{ column }; i < mValues.size(); i += columnsCount) {
				result.at(insertPos++) = mValues.at(i);
			}
		}

		return transpositioned_matrix{ std::move(result) };
	}

	[[nodiscard]] constexpr submat_type submat(const usize row, const usize column) const {
		if constexpr (std::is_same_v<submat_type, submat_error_value>) return {};
		if constexpr (columns() == 1 || rows() == 1) return {};
		if (columns() == 1 || rows() == 1) return {};

		submat_type result;
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

	template<class OtherValueType>
	[[nodiscard]] constexpr separated_matrix<OtherValueType> separate(
		const separate_matrix_param<OtherValueType> &other) const {
		using res_mat = separated_matrix<OtherValueType>;
		typename res_mat::values_container result{};
		#pragma omp parallel for
		for (usize row{}; row < rows(); ++row) {
			for (usize column{}; column < columns(); ++column) {
				result.at(static_cast<size_type>(res_mat::index(row, column)))
					= static_cast<OtherValueType>(at(row, column));
				result.at(static_cast<size_type>(res_mat::index(row, column + columns())))
					= other.at(row, column);
			}
		}
		return res_mat{ std::move(result) };
	}

	template<usize OtherColumns, usize OtherRows>
	[[nodiscard]] constexpr bool operator==(const mat<T, OtherColumns, OtherRows> &other) const {
		if constexpr (OtherColumns != columns() || OtherRows != rows()) return false;
		if (this == &other) return true;

		for (usize index{}; index < length(); ++index) {
			if constexpr (std::is_floating_point_v<T>) {
				if (!utils::numeric::approximately::are_equal(at(index), other.at(index))) {
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

	template<usize OtherColumns, usize OtherRows>
	[[nodiscard]] constexpr bool operator!=(const mat<T, OtherColumns, OtherRows> &other) const {
		if constexpr (OtherColumns != columns() || OtherRows != rows()) return true;
		return !(*this == other);
	}

	constexpr mat &operator+=(const mat &other) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(other.mValues), std::begin(mValues), std::plus<int>());
		mDeterminant = std::nullopt;
		return *this;
	}
	constexpr mat &operator-=(const mat &other) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(other.mValues), std::begin(mValues), std::minus<int>());
		mDeterminant = std::nullopt;
		return *this;
	}
	constexpr mat &operator*=(const T &value) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(mValues), [&value](const T &elem) { return elem * value; });
		mDeterminant = std::nullopt;
		return *this;
	}
	constexpr mat &operator/=(const T &value) {
		std::transform(std::execution::par, std::begin(mValues), std::end(mValues),
			std::begin(mValues), [&value](const T &elem) { return elem / value; });
		mDeterminant = std::nullopt;
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
	template<class OtherValueType>
	[[nodiscard]] constexpr separated_matrix<OtherValueType> operator|(
		const separate_matrix_param<OtherValueType> &other) const {
		return separate(other);
	}

	constexpr mat &operator*=(const mat &other) {
		return *this = (*this) * other;
	}

	template<size_t OtherColumns>
	[[nodiscard]] constexpr mat<T, OtherColumns, Dimension> operator*(const mat<T, Dimension, OtherColumns> &other) const {
		using result_matrix = mat<T, OtherColumns, Dimension>;
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

	[[nodiscard]] constexpr vec<T, Dimension> operator*(const vec<T, Dimension> &other) const {
		vec<T, Dimension> result{};

		#pragma omp parallel for
		for (usize row{}; row < rows(); ++row) {
			for (usize column{}; column < columns(); ++column) {
				result.at(static_cast<size_type>(row)) += at(row, column) * other.at(column);
			}
		}

		return result;
	}

	[[nodiscard]] constexpr mat operator-() const {
		return mat{ *this } *= -1;
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
	mutable std::optional<T> mDeterminant{};
	values_container mValues{ identityValues() };

	static constexpr values_container identityValues() {
		if constexpr (rows() == 1) return { 1 };
		else if constexpr (rows() == 2) return { 1, 0,  0, 1 };
		else if constexpr (rows() == 3) return { 1, 0, 0,  0, 1, 0,  0, 0, 1 };
		else if constexpr (rows() == 4) return { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
		else {
			values_container values{};
			constexpr size_type step{ static_cast<size_type>(columns() + 1) };
			#pragma omp parallel for
			for (size_type id{}; id < length(); id += step) {
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

template<class T, usize Dimension>
using smat = mat<T, Dimension, Dimension>;

template<class T, usize Dimension>
using square_mat = smat<T, Dimension>;

template<class T>
using mat2 = smat<T, 2>;

template<class T>
using mat3 = smat<T, 3>;

using mat3f32 = mat3<f32>;
using mat3f = mat3f32;
using mat3f64 = mat3<f64>;
using mat3d = mat3f64;
using mat3i32 = mat3<i32>;
using mat3i = mat3i32;

template<class T>
using mat4 = smat<T, 4>;

using mat4f32 = mat4<f32>;
using mat4f = mat4f32;
using mat4f64 = mat4<f64>;
using mat4d = mat4f64;
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
