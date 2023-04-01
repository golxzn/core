#include <spdlog/spdlog.h>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#include <golxzn/core/utils/error.hpp>
#include <golxzn/core/resources/manager.hpp>

TEST_CASE("Image", "[golxzn][core][tests]") {
	using namespace golxzn;
	core::res_man::initialize("core");

	SECTION("crop") {
		auto png{ core::res_man::load_image("res://images/test.png") };
		REQUIRE(png != nullptr);
		REQUIRE(png->width() == 300);
		REQUIRE(png->height() == 300);

		const auto crop_test = [&] (const auto rect) {
			const auto expected_width{ rect.width - rect.x };
			const auto expected_height{ rect.height - rect.y };
			png->crop(rect);
			REQUIRE(png->width() == expected_width);
			REQUIRE(png->height() == expected_height);
			const std::string path{"user://images/cropped/" +
				std::to_string(expected_width) + "x" +
				std::to_string(expected_height) + ".png"
			};
			core::res_man::save_image(path, png);
		};

		crop_test(core::rect<core::u32>{ 0, 0, 300, 250 });  // [300, 300] -> [300, 250]
		crop_test(core::rect<core::u32>{ 0, 0, 250, 250 });  // [300, 250] -> [250, 250]
		crop_test(core::rect<core::u32>{ 0, 50, 250, 250 }); // [250, 250] -> [250, 200]
		crop_test(core::rect<core::u32>{ 50, 0, 250, 200 }); // [250, 200] -> [200, 200]
		crop_test(core::rect<core::u32>{ 50, 50, 100, 100 }); // [200, 200] -> [100, 100]
	}

	SECTION("expand") {
		using namespace golxzn::core::color_literals;
		using namespace golxzn::core;

		auto png{ res_man::load_image("res://images/test.png") };

		const auto expand_test = [&] (auto clr, u32 x, u32 y, u32 w, u32 h) {
			const auto expected_width{ x + w + png->width() };
			const auto expected_height{ y + h + png->height() };
			png->expand(rect<u32>{ x, y, w, h }, clr);
			REQUIRE(png->width() == expected_width);
			REQUIRE(png->height() == expected_height);
			const std::string path{ "user://images/expanded/" +
				std::to_string(png->width()) + "x" +
				std::to_string(png->height()) + ".png"
			};
			res_man::save_image(path, png);
		};

		//             Color          left   up right down
		expand_test("red"_clr,           0, 100,   0,   0);  // [300, 300] -> [300, 400]
		expand_test("green"_clr,         0,   0,   0, 100);  // [300, 400] -> [300, 500]
		expand_test("silver"_clr,      100,   0,   0,   0);  // [300, 500] -> [400, 500]
		expand_test("dark_red"_clr,      0,   0, 100,   0);  // [400, 500] -> [500, 500]
		expand_test("blue"_clr,          0, 100, 100,   0);  // [500, 500] -> [600, 600]
		expand_test("gold"_clr,        100,   0,   0, 100);  // [600, 600] -> [700, 700]
		expand_test("white"_clr,       100, 100, 100, 100);  // [700, 700] -> [900, 900]
	}
}

// TEST_CASE("Image", "[golxzn][core][benchmarks]") {
// }

// int main(int argc, char* argv[]) {
// 	Catch::Session session;
// 	session.configData().benchmarkSamples = 10; // set number of iterations
// 	int returnCode = session.run(argc, argv);
// 	return returnCode;
// }
