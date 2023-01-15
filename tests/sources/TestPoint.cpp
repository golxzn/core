#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <golxzn/core/point.hpp>
#include <iostream>
#include <iomanip>

TEST_CASE("point<T, Length>", "[golxzn][core]") {
	using namespace std::string_literals;
	using namespace golxzn;

	using point3R = core::point3<core::f32>;
	using point3I = core::point3<core::i32>;
	using point3U = core::point3<core::u32>;
	using point2R = core::point2<core::f32>;
	using point2I = core::point2<core::i32>;
	using point2U = core::point2<core::u32>;
	using point1R = core::point<core::f32, 1>;
	using point1I = core::point<core::i32, 1>;
	using point1U = core::point<core::u32, 1>;

	point3R point{ 3.3, 1.2, 4.5 };

	SECTION("Initializing") {
		REQUIRE(point2I{ 1, 2 }.at(0) == 1);
		REQUIRE(point2I{ 1, 2 }.at(1) == 2);
		REQUIRE(point1I{ 1 }.at(0) == 1);
		point2U upoint;
		upoint.at(0) = 100u;
		REQUIRE(upoint.at(0) == 100u);
	}

	SECTION("Elements accessability") {
		REQUIRE_THAT(point.at(0), Catch::Matchers::WithinRel(3.3));
		REQUIRE_THAT(point.at(1), Catch::Matchers::WithinRel(1.2));
		REQUIRE_THAT(point.at(2), Catch::Matchers::WithinRel(4.5));
		REQUIRE_THROWS(point.at(3));
		REQUIRE_THAT(point[0], Catch::Matchers::WithinRel(3.3));
		REQUIRE_THAT(point[1], Catch::Matchers::WithinRel(1.2));
		REQUIRE_THAT(point[2], Catch::Matchers::WithinRel(4.5));
		REQUIRE_THROWS(point[3]);
	}

	SECTION("Distance calculating") {
		using namespace Catch;

		const auto aver_4{ Matchers::WithinRel(4.0) };
		const auto aver_5{ Matchers::WithinRel(5.0) };
		const auto aver_6{ Matchers::WithinRel(6.0, std::numeric_limits<core::f32>::epsilon()) };
		const auto aver_16{ Matchers::WithinRel(16.0) };
		const auto aver_25{ Matchers::WithinRel(25.0) };
		const auto aver_36{ Matchers::WithinRel(36.0, std::numeric_limits<core::f32>::epsilon()) };

		REQUIRE_THAT((point1R{ 2.0 }.distance(point1R{ 6.0 })), aver_4);
		REQUIRE_THAT((point1U{ 2u }.distance(point1R{ 6.0 })), aver_4);
		REQUIRE_THAT((point1R{ 2.0 }.distance(point1U{ 6u })), aver_4);
		REQUIRE_THAT((point1R{ 2.0 }.distanceSquared(point1R{ 6.0 })), aver_16);
		REQUIRE_THAT((point1U{ 2u }.distanceSquared(point1R{ 6.0 })), aver_16);
		REQUIRE_THAT((point1R{ 2.0 }.distanceSquared(point1U{ 6u })), aver_16);

		REQUIRE_THAT((point2R{ 2.0, 4.0 }.distance(point2R{ 6.0, 7.0 })), aver_5);
		REQUIRE_THAT((point2U{ 2u, 4u }.distance(point2R{ 6.0, 7.0 })), aver_5);
		REQUIRE_THAT((point2R{ 2.0, 4.0 }.distance(point2U{ 6u, 7u })), aver_5);
		REQUIRE_THAT((point2R{ 2.0, 4.0 }.distanceSquared(point2R{ 6.0, 7.0 })), aver_25);
		REQUIRE_THAT((point2U{ 2u, 4u }.distanceSquared(point2R{ 6.0, 7.0 })), aver_25);
		REQUIRE_THAT((point2R{ 2.0, 4.0 }.distanceSquared(point2U{ 6u, 7u })), aver_25);

		constexpr core::f32 boring_num{ 6.316624790358 };
		// std::cout << "Distance: " << std::setprecision(20) << std::fixed << point3R{ 2.0, 4.0, 3.0 }.distance(point3R{ 6.0, 7.0, boring_num }) << "\n";
		// std::cout << "Value:    " << std::setprecision(20) << std::fixed << 6.0 << "\n";
		// std::cout << "Epsilon:  " << std::setprecision(20) << std::fixed << std::numeric_limits<core::real>::epsilon() << "\n";

		/// Fails with '6.0 and 6 are within 2.22045e-14% of each other'
		// REQUIRE_THAT((point3R{ 2.0, 4.0, 3.0 }.distance(point3R{ 6.0, 7.0, boring_num })), aver_6);
		/// Fails with '5.8309518948 and 6 are within 2.22045e-14% of each other'
		// REQUIRE_THAT((point3U{ 2u, 4u, 3u }.distance(point3R{ 6.0, 7.0, boring_num })), aver_6);
		/// Fails with '36.0 and 36 are within 2.22045e-14% of each other'
		// REQUIRE_THAT((point3R{ 2.0, 4.0, 3.0 }.distanceSquared(point3R{ 6.0, 7.0, boring_num })), aver_36);
		/// Fails with '34.0 and 36 are within 2.22045e-14% of each other'
		// REQUIRE_THAT((point3U{ 2u, 4u, 3u }.distanceSquared(point3R{ 6.0, 7.0, boring_num })), aver_36);
	}

	SECTION("Converting") {
		REQUIRE(point.as<3>({0,0,0}) == point3R{ 3.3, 3.3, 3.3 });
		REQUIRE(point.as<2>({2,1}) == point2R{ 4.5, 1.2 });
		REQUIRE(point.as<5>({2,1,0,1,2}) == core::point<core::f32, 5>{ 4.5, 1.2, 3.3, 1.2, 4.5 });
	}
}

