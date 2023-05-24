// #include <spdlog/spdlog.h>
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/benchmark/catch_benchmark_all.hpp>

#include <golxzn/core/utils/error.hpp>
#include <golxzn/core/resources/manager.hpp>

TEST_CASE("Resource Manager", "[golxzn][core][tests]") {
	using namespace golxzn;

	SECTION("initilization") {
		core::res_man::initialize("core");
	}
	SECTION("image") {
		auto png{ core::res_man::load_image("res://images/test.png") };
		REQUIRE(png != nullptr);
		REQUIRE(png->width() == 300);
		REQUIRE(png->height() == 300);
		REQUIRE(core::res_man::save_image("user://test.png", png) == true);

		auto jpg{ core::res_man::load_image("res://images/test.jpg") };
		REQUIRE(jpg != nullptr);
		REQUIRE(jpg->width() == 2048);
		REQUIRE(jpg->height() == 2048);
		REQUIRE(core::res_man::save_image("user://images/original.jpg", jpg) == true);
		jpg->flip(core::types::image::flip_direction::both);
		REQUIRE(core::res_man::save_image("user://images/flipped/both.jpg", jpg) == true);
		jpg->flip(core::types::image::flip_direction::vertical);
		REQUIRE(core::res_man::save_image("user://images/flipped/vertically.tga", jpg) == true);
		jpg->flip(core::types::image::flip_direction::horizontal);
		REQUIRE(core::res_man::save_image("user://images/flipped/horizontally.bmp", jpg) == true);
	}
}

// TEST_CASE("Resource Manager", "[golxzn][core][benchmarks]") {
// 	using namespace golxzn;
// 	using namespace golxzn::core;
// 	using namespace golxzn::types_literals;

// 	struct for_bench_t {
// 		types::image::ref image;
// 		bool load_image(const std::string_view path) {
// 			image = res_man::load_image(path);
// 			return image != nullptr;
// 		}
// 		bool flip(bool vertical, bool horizontal) noexcept {
// 			if (vertical && horizontal) image->flip(types::image::flip_direction::both);
// 			else if (vertical)          image->flip(types::image::flip_direction::vertical);
// 			else if (horizontal)        image->flip(types::image::flip_direction::horizontal);
// 			return true;
// 		}
// 		bool save_image(const std::string_view path) const {
// 			return res_man::save_image(path, image);
// 		}
// 	} for_bench;

// 	BENCHMARK("load_image: res://images/test.jpg") { return for_bench.load_image("res://images/test.jpg");  };
// 	BENCHMARK("save_image: user://images/bench/test.jpg") { return for_bench.save_image("user://images/bench/test.jpg"); };
// 	BENCHMARK("flip v: true,  h: false")  { return for_bench.flip(true, false);  };
// 	BENCHMARK("save_image: user://images/bench/test.png") { return for_bench.save_image("user://images/bench/test.png"); };
// 	BENCHMARK("flip v: false, h: true ")  { return for_bench.flip(false, true);  };
// 	BENCHMARK("save_image: user://images/bench/test.bmp") { return for_bench.save_image("user://images/bench/test.bmp"); };
// 	BENCHMARK("flip v: true,  h: true ")  { return for_bench.flip(true, true);  };
// 	BENCHMARK("save_image: user://images/bench/test.tga") { return for_bench.save_image("user://images/bench/test.tga"); };
// }

// int main(int argc, char* argv[]) {
// 	Catch::Session session;
// 	session.configData().benchmarkSamples = 10; // set number of iterations
// 	int returnCode = session.run(argc, argv);
// 	return returnCode;
// }
