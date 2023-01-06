#include <iostream>
#include <functional>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <golxzn/core/matrix.hpp>

TEST_CASE("mat<T, Columns, Rows>", "[golxzn][core][tests]") {
	using namespace golxzn;
	using namespace std::placeholders;

	auto constant_num_gen{
		[](auto num, [[maybe_unused]] core::usize row, [[maybe_unused]] core::usize column) {
			return num;
		}
	};
	auto zero_num_gen{ std::bind(constant_num_gen, 0, _1, _2) };

	auto check_values = [&](auto name, auto matrix, auto value_generator) {
		for(core::usize row{}; row < matrix.rows(); ++row) {
			for(core::usize column{}; column < matrix.columns(); ++column) {
				REQUIRE(matrix.at(row, column) == value_generator(row, column));
				UNSCOPED_INFO("[" << name << "]" << "Row: " << row << ", Column: " << column);
			}
		}
	};

	constexpr core::mat3<core::i32> origin{
		2, 2, 2,
		2, 2, 2,
		2, 2, 2
	};
	constexpr core::mat3i32 sequential{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8
	};

	SECTION("Construct") {
		check_values("empty", core::mat3<core::real>::zero(), zero_num_gen);

		// Check identity matrix
		check_values("identity", core::mat3<core::i32>{}, [](core::usize row, core::usize column) {
			return (column == row ? 1 : 0);
		});

		check_values("sequential_matrix", sequential, [](core::usize row, core::usize column) {
			return core::mat3i32::index(row, column);
		});
	}
	SECTION("Identities") {
		auto generator = [](core::usize row, core::usize column) { return (column == row ? 1 : 0); };
		check_values("mat<1, 1>", core::mat<core::i32, 1, 1>::identity(), generator);
		check_values("mat<2, 2>", core::mat<core::i32, 2, 2>::identity(), generator);
		check_values("mat<3, 3>", core::mat<core::i32, 3, 3>::identity(), generator);
		check_values("mat<4, 4>", core::mat<core::i32, 4, 4>::identity(), generator);
		check_values("mat<5, 5>", core::mat<core::i32, 5, 5>::identity(), generator);
		check_values("mat<10, 10>", core::mat<core::i32, 10, 10>::identity(), generator);
	}
	SECTION("Indexes and value getting") {
		REQUIRE(core::mat3i32::index(0, 0) == 0);
		REQUIRE(core::mat3i32::index(0, 1) == 1);
		REQUIRE(core::mat3i32::index(1, 0) == 3);
	}
	SECTION("Equality") {
		REQUIRE(core::mat3i32::identity() == core::mat3i32::identity());
		REQUIRE_FALSE(core::mat3i32::identity() == core::mat3i32::zero());

		REQUIRE(core::mat3r::identity() == core::mat3r::identity());
		REQUIRE_FALSE(core::mat3r::identity() == core::mat3r::zero());
	}
	SECTION("Reversing") {
		constexpr core::mat3<core::i32> seq_reversed{
			0, 3, 6,
			1, 4, 7,
			2, 5, 8
		};
		REQUIRE_FALSE(sequential == seq_reversed);
		REQUIRE(sequential.reverse() == seq_reversed);

		core::mat2<core::i32> orig{ 0, 1, 2, 3 };
		core::mat2<core::i32> copy{ orig };
		REQUIRE(orig == copy);
		orig.reverse();
		REQUIRE_FALSE(orig == copy);
		copy.reverse();
		REQUIRE(orig == copy);

		core::mat<core::i32, 2, 1> vec{ 1, 2 };
		REQUIRE_THROWS(vec.reverse());
	}
	SECTION("Transposition") {
		const core::mat<core::i32, 2, 3> origin{
			1, 2,
			3, 4,
			5, 6
		};
		const core::mat<core::i32, 3, 2> trans{
			1, 3, 5,
			2, 4, 6
		};
		REQUIRE(origin.transposition() == trans);
		const core::mat<core::i32, 1, 3> test{
			1,
			2,
			3,
		};
	}
	SECTION("Submatrix") {
		const auto sub{ sequential.subMatrix(1, 1) };
		REQUIRE(sub.rows() == 2);
		REQUIRE(sub.columns() == 2);
		check_values("subMatrixrix", sub, [](core::usize row, core::usize column) {
			static constexpr std::initializer_list<core::i32> values{
				0, 2,
				6, 8
			};
			return values.begin()[core::mat2<core::i32>::index(row, column)];
		});

		const core::mat2<core::i32> small{ 0, 1, 2, 3 };
		const auto subsmall{ small.subMatrix(0, 0) };
		REQUIRE(subsmall.rows() == 1);
		REQUIRE(subsmall.columns() == 1);
		REQUIRE(subsmall.at(0, 0) == 3);
	}
	SECTION("Summation") {
		core::mat3<core::i32> copy{ origin };
		copy += core::mat3<core::i32>{
			3, 3, 3,
			3, 3, 3,
			3, 3, 3
		};
		check_values("op +=", copy, std::bind(constant_num_gen, 5, _1, _2));
		check_values("op +", copy + copy, std::bind(constant_num_gen, 10, _1, _2));
	}
	SECTION("Subtraction") {
		core::mat3<core::i32> copy{ origin };
		copy -= core::mat3<core::i32>{
			1, 1, 1,
			1, 1, 1,
			1, 1, 1,
		};
		check_values("op -=", copy, std::bind(constant_num_gen, 1, _1, _2));
		check_values("op -", copy - copy, zero_num_gen);
	}
	SECTION("Multiplication") {
		core::mat3<core::i32> copy{ origin };
		copy *= 5;
		check_values("op *=", copy, std::bind(constant_num_gen, 10, _1, _2));
		check_values("op *", copy * 2, std::bind(constant_num_gen, 20, _1, _2));
	}
	SECTION("Division") {
		core::mat3<core::i32> copy{ origin * 5 };
		copy /= 2;
		check_values("op /=", copy, std::bind(constant_num_gen, 5, _1, _2));
		check_values("op /", copy / 5, std::bind(constant_num_gen, 1, _1, _2));
	}
	SECTION("Range loop") {
		for (const auto &value : origin) {
			REQUIRE(value == 2);
		}
	}
}

TEST_CASE("mat<T, Columns, Rows>", "[golxzn][core][benchmarks]") {
	using namespace golxzn;
	constexpr size_t columns{ 100 };
	std::array<core::i32, columns * columns> values;
	std::iota(std::begin(values), std::end(values), 1);
	core::mat<core::i32, columns, columns> forBench{ std::move(values) };
	const auto forBenchCopy{ forBench };

	BENCHMARK("mat<i32, 100, 100> Transposition")  { return forBench.transposition();  };
	BENCHMARK("mat<i32, 100, 100> Reverse")        { return forBench.reverse();        };
	BENCHMARK("mat<i32, 100, 100> Reverse (copy)") { return forBenchCopy.reverse();    };
}
