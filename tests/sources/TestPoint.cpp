#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <golxzn/core/point.hpp>
#include <iostream>
#include <iomanip>

TEST_CASE("Point<T, Length>", "[golxzn][core]") {
	using namespace std::string_literals;
	using namespace golxzn;

	using Point3R = core::Point3<core::real>;
	using Point3I = core::Point3<core::i32>;
	using Point3U = core::Point3<core::u32>;
	using Point2R = core::Point2<core::real>;
	using Point2I = core::Point2<core::i32>;
	using Point2U = core::Point2<core::u32>;
	using Point1R = core::Point<core::real, 1>;
	using Point1I = core::Point<core::i32, 1>;
	using Point1U = core::Point<core::u32, 1>;

	Point3R point{ 3.3, 1.2, 4.5 };

	SECTION("Initializing") {
		REQUIRE(Point2I{ 1, 2 }.at(0) == 1);
		REQUIRE(Point2I{ 1, 2 }.at(1) == 2);
		REQUIRE(Point1I{ 1 }.at(0) == 1);
		Point2U upoint;
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
		constexpr core::real sad_moment{ 100000 };

		const auto aver_4{ Matchers::WithinRel(4.0) };
		const auto aver_5{ Matchers::WithinRel(5.0) };
		const auto aver_6{ Matchers::WithinRel(6.0, std::numeric_limits<core::real>::epsilon() * sad_moment) };
		const auto aver_16{ Matchers::WithinRel(16.0) };
		const auto aver_25{ Matchers::WithinRel(25.0) };
		const auto aver_36{ Matchers::WithinRel(36.0, std::numeric_limits<core::real>::epsilon() * sad_moment) };

		REQUIRE_THAT((Point1R{ 2.0 }.distance(Point1R{ 6.0 })), aver_4);
		REQUIRE_THAT((Point1U{ 2u }.distance(Point1R{ 6.0 })), aver_4);
		REQUIRE_THAT((Point1R{ 2.0 }.distance(Point1U{ 6u })), aver_4);
		REQUIRE_THAT((Point1R{ 2.0 }.distanceSquared(Point1R{ 6.0 })), aver_16);
		REQUIRE_THAT((Point1U{ 2u }.distanceSquared(Point1R{ 6.0 })), aver_16);
		REQUIRE_THAT((Point1R{ 2.0 }.distanceSquared(Point1U{ 6u })), aver_16);

		REQUIRE_THAT((Point2R{ 2.0, 4.0 }.distance(Point2R{ 6.0, 7.0 })), aver_5);
		REQUIRE_THAT((Point2U{ 2u, 4u }.distance(Point2R{ 6.0, 7.0 })), aver_5);
		REQUIRE_THAT((Point2R{ 2.0, 4.0 }.distance(Point2U{ 6u, 7u })), aver_5);
		REQUIRE_THAT((Point2R{ 2.0, 4.0 }.distanceSquared(Point2R{ 6.0, 7.0 })), aver_25);
		REQUIRE_THAT((Point2U{ 2u, 4u }.distanceSquared(Point2R{ 6.0, 7.0 })), aver_25);
		REQUIRE_THAT((Point2R{ 2.0, 4.0 }.distanceSquared(Point2U{ 6u, 7u })), aver_25);

		constexpr core::real boring_num{ 6.316624790358 };
		std::cout << "Distance: " << std::setprecision(20) << std::fixed << Point3R{ 2.0, 4.0, 3.0 }.distance(Point3R{ 6.0, 7.0, boring_num }) << "\n";
		std::cout << "Value:    " << std::setprecision(20) << std::fixed << 6.0 << "\n";
		std::cout << "Epsilon:  " << std::setprecision(20) << std::fixed << std::numeric_limits<core::real>::epsilon() * sad_moment << "\n";

		REQUIRE_THAT((Point3R{ 2.0, 4.0, 3.0 }.distance(Point3R{ 6.0, 7.0, boring_num })), aver_6);
		REQUIRE_THAT((Point3U{ 2u, 4u, 3u }.distance(Point3R{ 6.0, 7.0, boring_num })), aver_6);
		REQUIRE_THAT((Point3R{ 2.0, 4.0, 3.0 }.distanceSquared(Point3R{ 6.0, 7.0, boring_num })), aver_36);
		REQUIRE_THAT((Point3U{ 2u, 4u, 3u }.distanceSquared(Point3R{ 6.0, 7.0, boring_num })), aver_36);
	}

	SECTION("Converting") {
		REQUIRE(point.as<3>({0,0,0}) == Point3R{ 3.3, 3.3, 3.3 });
		REQUIRE(point.as<2>({2,1}) == Point2R{ 4.5, 1.2 });
		REQUIRE(point.as<5>({2,1,0,1,2}) == core::Point<core::real, 5>{ 4.5, 1.2, 3.3, 1.2, 4.5 });
	}
}

