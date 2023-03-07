#include <catch2/catch_test_macros.hpp>
#include <golxzn/core/types/color.hpp>
#include <iostream>

TEST_CASE("Color", "[types]") {
	using namespace golxzn;
	using namespace golxzn::types_literals;

	SECTION("Constructors") {
		const core::color defaultConstructed;
		REQUIRE(defaultConstructed.r == 255_u8);
		REQUIRE(defaultConstructed.g == 255_u8);
		REQUIRE(defaultConstructed.b == 255_u8);
		REQUIRE(defaultConstructed.a == 255_u8);

		const core::color initializerList{ 10_u8, 20_u8, 30_u8, 40_u8 };
		REQUIRE(initializerList.r == 10_u8);
		REQUIRE(initializerList.g == 20_u8);
		REQUIRE(initializerList.b == 30_u8);
		REQUIRE(initializerList.a == 40_u8);

		const core::color floatConstructor{ 0.1_f32, 0.2_f32, 0.3_f32, 0.4_f32 };
		REQUIRE(floatConstructor.r == 25_u8);
		REQUIRE(floatConstructor.g == 51_u8);
		REQUIRE(floatConstructor.b == 76_u8);
		REQUIRE(floatConstructor.a == 102_u8);
	}

	SECTION("make_color") {
		using namespace golxzn::core::color_literals;

		REQUIRE(core::validate_hex_color("#FF0000") == true);
		REQUIRE(core::validate_hex_color("#FFFF0000") == true);
		REQUIRE(core::validate_hex_color("#FFap000") == false);
		REQUIRE(core::validate_hex_color("#lolkekcheburek") == false);

		const auto white{ "white"_clr };
		REQUIRE(white == core::color{ 255_u8, 255_u8, 255_u8, 255_u8 });

		const auto hexWithoutAlpha{ core::make_color("#FF0000") };
		REQUIRE(hexWithoutAlpha == core::color{ 255_u8, 0_u8, 0_u8, 255_u8 });

		const auto hexWithAlpha{ "#FE00FF00"_clr };
		REQUIRE(hexWithAlpha == core::color{ 0_u8, 255_u8, 0_u8, 254_u8 });
	}
}