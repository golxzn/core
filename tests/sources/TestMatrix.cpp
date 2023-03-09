#include <functional>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <golxzn/core/utils/error.hpp>
#include <golxzn/core/math/matrix.hpp>

TEST_CASE("mat<T, Columns, Rows>", "[golxzn][core][tests]") {
	using namespace golxzn;
	using namespace golxzn::types_literals;
	using namespace std::placeholders;

	auto constant_num_gen{
		[](auto num, [[maybe_unused]] core::usize row, [[maybe_unused]] core::usize column) {
			return num;
		}
	};
	auto zero_num_gen{ std::bind(constant_num_gen, 0_usize, _1, _2) };

	auto check_values = [&](auto name, auto matrix, auto value_generator) {
		for(core::usize row{}; row < matrix.rows(); ++row) {
			for(core::usize column{}; column < matrix.columns(); ++column) {
				REQUIRE(matrix.at(row, column) == value_generator(row, column));
				UNSCOPED_INFO("[" << name << "]" << "Row: " << row << ", Column: " << column);
			}
		}
	};

	constexpr core::mat3<core::i32> origin{
		2_i32, 2_i32, 2_i32,
		2_i32, 2_i32, 2_i32,
		2_i32, 2_i32, 2_i32
	};
	constexpr core::mat3i32 sequential{
		0_i32, 1_i32, 2_i32,
		3_i32, 4_i32, 5_i32,
		6_i32, 7_i32, 8_i32
	};

	SECTION("Construct") {
		check_values("empty", core::mat3<core::f32>::zero(), zero_num_gen);

		// Check identity matrix
		check_values("identity", core::mat3<core::i32>{}, [](core::usize row, core::usize column) {
			return (column == row ? 1_i32 : 0_i32);
		});

		check_values("sequential_matrix", sequential, [](core::usize row, core::usize column) {
			return core::mat3i32::index(row, column);
		});
	}
	SECTION("Identities") {
		auto generator = [](core::usize row, core::usize column) { return (column == row ? 1 : 0); };
		check_values("mat<1, 1>", core::square_mat<core::i32, 1>::identity(), generator);
		check_values("mat<2, 2>", core::square_mat<core::i32, 2>::identity(), generator);
		check_values("mat<3, 3>", core::square_mat<core::i32, 3>::identity(), generator);
		check_values("mat<4, 4>", core::square_mat<core::i32, 4>::identity(), generator);
		check_values("mat<5, 5>", core::square_mat<core::i32, 5>::identity(), generator);
		check_values("mat<10, 10>", core::square_mat<core::i32, 10>::identity(), generator);
	}
	SECTION("Indexes and value getting") {
		REQUIRE(core::mat3i32::index(0_usize, 0_usize) == 0_i32);
		REQUIRE(core::mat3i32::index(0_usize, 1_usize) == 1_i32);
		REQUIRE(core::mat3i32::index(1_usize, 0_usize) == 3_i32);
	}
	SECTION("Equality") {
		REQUIRE(core::mat3i32::identity() == core::mat3i32::identity());
		REQUIRE_FALSE(core::mat3i32::identity() == core::mat3i32::zero());

		REQUIRE(core::mat3f64::identity() == core::mat3f64::identity());
		REQUIRE_FALSE(core::mat3f64::identity() == core::mat3f64::zero());
	}
	SECTION("Reversing") {
		constexpr core::mat3<core::i32> seq_reversed{
			0_i32, 3_i32, 6_i32,
			1_i32, 4_i32, 7_i32,
			2_i32, 5_i32, 8_i32
		};
		REQUIRE_FALSE(sequential == seq_reversed);
		REQUIRE(sequential.reverse() == seq_reversed);

		core::mat2<core::i32> orig{ 0_i32, 1_i32, 2_i32, 3_i32 };
		core::mat2<core::i32> copy{ orig };
		REQUIRE(orig == copy);
		orig.reverse();
		REQUIRE_FALSE(orig == copy);
		copy.reverse();
		REQUIRE(orig == copy);

		if constexpr (core::utils::error::exceptions_enabled) {
			core::mat<core::i32, 1, 2> vec{ 1_i32, 2_i32 };
			REQUIRE_THROWS(vec.reverse());
		}
	}
	SECTION("Transposition") {
		const core::mat<core::i32, 3, 2> orig{
			1_i32, 2_i32,
			3_i32, 4_i32,
			5_i32, 6_i32
		};
		const core::mat<core::i32, 2, 3> trans{
			1_i32, 3_i32, 5_i32,
			2_i32, 4_i32, 6_i32
		};
		REQUIRE(orig.transposition() == trans);
		const core::mat<core::i32, 3, 1> test{
			1_i32,
			2_i32,
			3_i32
		};
		REQUIRE(test.transposition() == core::mat<core::i32, 1, 3>{ 1_i32, 2_i32, 3_i32 });
	}
	SECTION("Submatrix") {
		const auto sub{ sequential.submat(1_usize, 1_usize) };
		REQUIRE(sub.rows() == 2_i32);
		REQUIRE(sub.columns() == 2_i32);
		check_values("subMatrixrix", sub, [](core::usize row, core::usize column) {
			static constexpr std::initializer_list<core::i32> values{
				0_i32, 2_i32,
				6_i32, 8_i32
			};
			return values.begin()[core::mat2<core::i32>::index(row, column)];
		});

		const core::mat2<core::i32> small_mat{ 0_i32, 1_i32, 2_i32, 3_i32 };
		const auto subsmall{ small_mat.submat(0_usize, 0_usize) };
		REQUIRE(subsmall.rows() == 1_i32);
		REQUIRE(subsmall.columns() == 1_i32);
		REQUIRE(subsmall.at(0_usize, 0_usize) == 3_i32);
	}
	SECTION("Summation") {
		core::mat3<core::i32> copy{ origin };
		copy += core::mat3<core::i32>{
			3_i32, 3_i32, 3_i32,
			3_i32, 3_i32, 3_i32,
			3_i32, 3_i32, 3_i32
		};
		check_values("op +=", copy, std::bind(constant_num_gen, 5_usize, _1, _2));
		check_values("op +", copy + copy, std::bind(constant_num_gen, 10_usize, _1, _2));
	}
	SECTION("Subtraction") {
		core::mat3<core::i32> copy{ origin };
		copy -= core::mat3<core::i32>{
			1_i32, 1_i32, 1_i32,
			1_i32, 1_i32, 1_i32,
			1_i32, 1_i32, 1_i32,
		};
		check_values("op -=", copy, std::bind(constant_num_gen, 1_usize, _1, _2));
		check_values("op -", copy - copy, zero_num_gen);
	}
	SECTION("Multiplication") {
		core::mat3<core::i32> copy{ origin };
		copy *= 5_i32;
		check_values("op *=", copy, std::bind(constant_num_gen, 10_usize, _1, _2));
		check_values("op *", copy * 2_i32, std::bind(constant_num_gen, 20_usize, _1, _2));

		check_values("op * (matrix)", origin * sequential, [](core::usize row, core::usize column) {
			static constexpr core::mat3<core::i32> result{
				18_i32, 24_i32, 30_i32,
				18_i32, 24_i32, 30_i32,
				18_i32, 24_i32, 30_i32
			};
			return result.at(row, column);
		});
		const core::mat<core::i32, 3, 2> A{
			1_i32, 2_i32,
			3_i32, 4_i32,
			5_i32, 6_i32
		};
		const core::mat<core::i32, 2, 3> B{
			7_i32, 8_i32, 9_i32,
			10_i32, 11_i32, 12_i32
		};
		const core::mat<core::i32, 3, 3> C{ A * B };

		check_values("op * (matrix)", C, [](core::usize row, core::usize column) {
			static constexpr const core::mat3<core::i32> result{
				27_i32,  30_i32,  33_i32,
				61_i32,  68_i32,  75_i32,
				95_i32, 106_i32, 117_i32
			};
			return result.at(row, column);
		});
		const auto one{ core::smat<core::i32, 1>{ 5_i32 } * core::smat<core::i32, 1>{ 10_i32 } };
		REQUIRE(one.at(0_usize, 0_usize) == 50_i32);
	}
	SECTION("Division") {
		core::mat3<core::i32> copy{ origin * 5_i32 };
		copy /= 2_i32;
		check_values("op /=", copy, std::bind(constant_num_gen, 5_usize, _1, _2));
		check_values("op /", copy / 5_i32, std::bind(constant_num_gen, 1_usize, _1, _2));
	}
	SECTION("Range loop") {
		for (const auto &value : origin) {
			REQUIRE(value == 2_i32);
		}
	}
	SECTION("Determinant") {
		REQUIRE(core::mat2<core::i32>{ 3_i32, 7_i32, 1_i32, -4_i32 }.determinant() == -19_i32);
		REQUIRE(core::mat<core::i32, 2, 3>{ 0_i32, 1_i32, 2_i32, 3_i32, 4_i32, 5_i32 }.determinant() == 0_i32);
	}
	SECTION("Swap") {
		auto copy{ sequential };
		copy.swap(0_usize, 1_usize);
		REQUIRE(copy == core::mat3i32{ 3_i32, 4_i32, 5_i32, 0_i32, 1_i32, 2_i32, 6_i32, 7_i32, 8_i32 });
		copy.swap(1_usize, 2_usize, decltype(copy)::Target::Columns);
		REQUIRE(copy == core::mat3i32{ 3_i32, 5_i32, 4_i32, 0_i32, 2_i32, 1_i32, 6_i32, 8_i32, 7_i32 });
	}
	SECTION("Separate") {
		const core::mat3i32::separated_matrix<core::i32> result{
			2_i32, 2_i32, 2_i32, 0_i32, 1_i32, 2_i32,
			2_i32, 2_i32, 2_i32, 3_i32, 4_i32, 5_i32,
			2_i32, 2_i32, 2_i32, 6_i32, 7_i32, 8_i32
		};
		REQUIRE(origin.separate(sequential) == result);

	}
	SECTION("Inverse") {
		const core::mat3<core::f32> matrix{
			3.0_f32,  0.0_f32,  2.0_f32,
			2.0_f32,  0.0_f32, -2.0_f32,
			0.0_f32,  1.0_f32,  1.0_f32
		};
		const core::mat3<core::f32> result{
			 0.2_f32,  0.2_f32,  0.0_f32,
			-0.2_f32,  0.3_f32,  1.0_f32,
			 0.2_f32, -0.3_f32,  0.0_f32
		};
		const auto optional{ matrix.inverse() };
		REQUIRE(optional.has_value());
		REQUIRE(optional.value() == result);
	}
}

template<class T, golxzn::core::usize Columns, golxzn::core::usize Rows>
inline constexpr golxzn::core::mat<T, Rows, Columns> make_mat() {
	std::array<T, Columns * Rows> values;
	std::iota(std::begin(values), std::end(values), static_cast<T>(1));
	return golxzn::core::mat<T, Rows, Columns>{ std::move(values) };
}

TEST_CASE("mat<T, Columns, Rows>", "[golxzn][core][benchmarks]") {
	using namespace golxzn;
	using namespace golxzn::types_literals;

	constexpr auto columns{ 100_usize };
	auto forBench{ make_mat<core::i32, columns, columns>() };
	const auto forBenchCopy{ forBench };

	constexpr auto detCols{ 10_usize };
	auto forDeterminant{ make_mat<core::i32, detCols, detCols>() };
	auto forInvalidDeterminant{ make_mat<core::i32, detCols, detCols + 1>() };

	// BENCHMARK("mat<i32, 100, 100> Transposition")  { return forBench.transposition();  };
	// BENCHMARK("mat<i32, 100, 100> Reverse")        { return forBench.reverse();        };
	// BENCHMARK("mat<i32, 100, 100> Reverse (copy)") { return forBenchCopy.reverse();    };
	// BENCHMARK("mat<i32, 100, 100> multiplication") { return forBench * forBenchCopy;   };
	// BENCHMARK("mat<i32, 12, 13> determinant")      { return forInvalidDeterminant.determinant(); };
	// BENCHMARK("mat<i32, 12, 12> determinant")      { return forDeterminant.determinant();    };
}
