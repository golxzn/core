#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <golxzn/core/matrix.hpp>

TEST_CASE("Matrix<T, Columns, Rows>", "[golxzn][core]") {
	using namespace golxzn;

	auto check_values = [&](auto name, auto matrix, auto value_generator) {
		for(core::usize row{}; row < matrix.rows(); ++row) {
			for(core::usize column{}; column < matrix.columns(); ++column) {
				REQUIRE(matrix.at(row, column) == value_generator(row, column));
				UNSCOPED_INFO("[" << name << "]" << "Row: " << row << ", Column: " << column);
			}
		}
	};

	SECTION("Construct") {
		check_values("empty", core::mat3<core::real>::zero(),
			[]([[maybe_unused]] core::usize row, [[maybe_unused]] core::usize column) { return 0.0; });

		// Check identity matrix
		check_values("identity", core::mat3<core::i32>{}, [](core::usize row, core::usize column) {
			return (column == row ? 1 : 0);
		});

		core::mat3<core::i32> int3Matrix{
			0, 1, 2,
			3, 4, 5,
			6, 7, 8
		};
		check_values("sequential_matrix", int3Matrix, [](core::usize row, core::usize column) {
			return core::mat3<core::i32>::index(row, column);
		});
	}
	SECTION("Identities") {
		auto generator = [](core::usize row, core::usize column) { return (column == row ? 1 : 0); };
		check_values("mat<1, 1>", core::Matrix<core::i32, 1, 1>::identity(), generator);
		check_values("mat<2, 2>", core::Matrix<core::i32, 2, 2>::identity(), generator);
		check_values("mat<3, 3>", core::Matrix<core::i32, 3, 3>::identity(), generator);
		check_values("mat<4, 4>", core::Matrix<core::i32, 4, 4>::identity(), generator);
		check_values("mat<5, 5>", core::Matrix<core::i32, 5, 5>::identity(), generator);
		check_values("mat<10, 10>", core::Matrix<core::i32, 10, 10>::identity(), generator);
	}
	SECTION("Indexes and value getting") {
		REQUIRE(core::mat3<core::i32>::index(0, 0) == 0);
		REQUIRE(core::mat3<core::i32>::index(0, 1) == 1);
		REQUIRE(core::mat3<core::i32>::index(1, 0) == 3);
	}
	SECTION("Equality") {
		REQUIRE(core::mat3<core::i32>::identity() == core::mat3<core::i32>::identity());
		REQUIRE_FALSE(core::mat3<core::i32>::identity() == core::mat3<core::i32>::zero());

		REQUIRE(core::mat3<core::real>::identity() == core::mat3<core::real>::identity());
		REQUIRE_FALSE(core::mat3<core::real>::identity() == core::mat3<core::real>::zero());
	}
}
