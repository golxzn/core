#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <golxzn/core/math/point.hpp>
// #include <iostream>
// #include <iomanip>

TEST_CASE("point<T, Count>", "[golxzn][core]") {
	using namespace std::string_literals;
	using namespace golxzn::types_literals;
	using namespace golxzn::core;

	point3f test_val{ 3.3_f32, 1.2_f32, 4.5_f32 };

	SECTION("Initializing") {
		REQUIRE(point2i{ 1_i32, 2_i32 }.at(0) == 1_i32);
		REQUIRE(point2i{ 1_i32, 2_i32 }.at(1) == 2_i32);
		point2u upoint;
		upoint.at(0_usize) = 100_u32;
		REQUIRE(upoint.at(0_usize) == 100_u32);
	}

	SECTION("Elements accessability") {
		REQUIRE_THAT(test_val.at(0_usize), Catch::Matchers::WithinRel(3.3_f32));
		REQUIRE_THAT(test_val.at(1_usize), Catch::Matchers::WithinRel(1.2_f32));
		REQUIRE_THAT(test_val.at(2_usize), Catch::Matchers::WithinRel(4.5_f32));
		REQUIRE_THROWS(test_val.at(3_usize));
		REQUIRE_THAT(test_val[0], Catch::Matchers::WithinRel(3.3_f32));
		REQUIRE_THAT(test_val[1], Catch::Matchers::WithinRel(1.2_f32));
		REQUIRE_THAT(test_val[2], Catch::Matchers::WithinRel(4.5_f32));
		REQUIRE_THROWS(test_val[3]);
	}

	SECTION("Distance calculating") {
		using namespace Catch;

		const auto aver_5{ Matchers::WithinRel(5.0_f32) };
		const auto aver_6{ Matchers::WithinRel(6.0_f32, std::numeric_limits<f32>::epsilon()) };
		const auto aver_25{ Matchers::WithinRel(25.0_f32) };
		const auto aver_36{ Matchers::WithinRel(36.0_f32, std::numeric_limits<f32>::epsilon()) };

		REQUIRE_THAT((point2f{ 2.0_f32, 4.0_f32 }.distance(point2f{ 6.0_f32, 7.0_f32 })), aver_5);
		REQUIRE_THAT((point2u{ 2_u32, 4_u32 }.distance(point2f{ 6.0_f32, 7.0_f32 })), aver_5);
		REQUIRE_THAT((point2f{ 2.0_f32, 4.0_f32 }.distance(point2u{ 6_u32, 7_u32 })), aver_5);
		REQUIRE_THAT((point2f{ 2.0_f32, 4.0_f32 }.distance_squared(point2f{ 6.0_f32, 7.0_f32 })), aver_25);
		REQUIRE_THAT((point2u{ 2_u32, 4_u32 }.distance_squared(point2f{ 6.0_f32, 7.0_f32 })), aver_25);
		REQUIRE_THAT((point2f{ 2.0_f32, 4.0_f32 }.distance_squared(point2u{ 6_u32, 7_u32 })), aver_25);

		constexpr auto boring_num{ 6.316624790358_f32 };
		// std::cout << "Distance: " << std::setprecision(20) << std::fixed << point3f{ 2.0_f32, 4.0_f32, 3.0_f32 }.distance(point3f{ 6.0_f32, 7.0_f32, boring_num }) << "\n";
		// std::cout << "Value:    " << std::setprecision(20) << std::fixed << 6.0_f32 << "\n";
		// std::cout << "Epsilon:  " << std::setprecision(20) << std::fixed << std::numeric_limits<core::real>::epsilon() << "\n";

		/// Fails with '6.0 and 6 are within 2.22045e-14% of each other'
		REQUIRE_THAT((point3f{ 2.0_f32, 4.0_f32, 3.0_f32 }.distance(point3f{ 6.0_f32, 7.0_f32, boring_num })), aver_6);
		/// Fails with '5.8309518948 and 6 are within 2.22045e-14% of each other'
		REQUIRE_THAT((point3u{ 2_u32, 4_u32, 3_u32 }.distance(point3f{ 6.0_f32, 7.0_f32, boring_num })), aver_6);
		/// Fails with '36.0 and 36 are within 2.22045e-14% of each other'
		REQUIRE_THAT((point3f{ 2.0_f32, 4.0_f32, 3.0_f32 }.distance_squared(point3f{ 6.0_f32, 7.0_f32, boring_num })), aver_36);
		/// Fails with '34.0_f32 and 36 are within 2.22045e-14% of each other'
		REQUIRE_THAT((point3u{ 2_u32, 4_u32, 3_u32 }.distance_squared(point3f{ 6.0_f32, 7.0_f32, boring_num })), aver_36);
	}

	SECTION("Converting") {
		REQUIRE(test_val.as<3>({0,0,0}) == point3f{ 3.3_f32, 3.3_f32, 3.3_f32 });
		REQUIRE(test_val.as<2>({2,1}) == point2f{ 4.5_f32, 1.2_f32 });
		REQUIRE(test_val.as<5>({2,1,0,1,2}) == point<f32, 5>{ 4.5_f32, 1.2_f32, 3.3_f32, 1.2_f32, 4.5_f32 });
	}

	SECTION("Operators") {
		point3i32 value{ 2_i32, 4_i32, 6_i32 };
		REQUIRE(value * 5 == point3i32{ 10_i32, 20_i32, 30_i32 });
		REQUIRE(value / 2 == point3i32{ 1_i32, 2_i32, 3_i32 });

		REQUIRE(5 * value == point3i32{ 10_i32, 20_i32, 30_i32 });
		REQUIRE(2 / value == point3i32{ 1_i32, 2_i32, 3_i32 });

		value *= 5; REQUIRE(value == point3i32{ 10_i32, 20_i32, 30_i32 });
		value /= 5; REQUIRE(value == point3i32{ 2_i32, 4_i32, 6_i32 });

		REQUIRE(-value == point3i32{ -2_i32, -4_i32, -6_i32 });

		constexpr point2f32 value2{ 3.0_f32, 4.0_f32 };
		REQUIRE(value2.length_no_squared() == 25);
		REQUIRE(value2.length() == 5);
		REQUIRE(value2.dot_product(point2f32{ 10.0_f32, 20.0_f32 }) == 110.0_f32);
	}
}

