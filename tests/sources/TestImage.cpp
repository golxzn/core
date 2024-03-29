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
	using namespace golxzn::core;
	using namespace golxzn::types_literals;
	using namespace golxzn::core::color_literals;
	res_man::initialize("core");

	SECTION("crop") {
		auto png{ res_man::load_image("res://images/test.png") };
		REQUIRE(png != nullptr);
		REQUIRE(png->width() == 300);
		REQUIRE(png->height() == 300);

		const auto crop_test = [&] (const auto rect) {
			png->crop(rect);
			REQUIRE(png->width() == rect.width);
			REQUIRE(png->height() == rect.height);
			res_man::save_image(std::string{"user://images/cropped/" +
				std::to_string(rect.width) + "x" +
				std::to_string(rect.height) + ".png"
			}, png);
		};

		crop_test(rect<u32>{  0,  0, 300, 250 }); // [300, 300] -> [300, 250]
		crop_test(rect<u32>{  0,  0, 250, 250 }); // [300, 250] -> [250, 250]
		crop_test(rect<u32>{  0, 50, 250, 200 }); // [250, 250] -> [250, 200]
		crop_test(rect<u32>{ 50,  0, 200, 200 }); // [250, 200] -> [200, 200]
		crop_test(rect<u32>{ 50, 50, 100, 100 }); // [200, 200] -> [100, 100]
	}

	SECTION("expand") {

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

	SECTION("copy") {
		static constexpr std::string_view src_image_path{ "res://images/test.jpg" };
		using overlap = types::image::overlap_policy;
		using p = types::image::position;

		auto source{ res_man::load_image(src_image_path) };
		const auto target{ res_man::load_image("res://images/test.png") };

		u32 id{};

		const auto copy_test = [&] (p at, rect<u32> src_rect, overlap policy, bool alpha = false, bool save = false) {
			source->copy(at, target, src_rect, true, policy);
			if (!save) return;

			std::string name{ "user://images/copied/" };
			name += std::to_string(id++);
			// name += (alpha ? "-merge_alpha" : "-copy_alpha");
			name += (policy == overlap::discard_target ? "-discard.png" : "-expand.png");
			res_man::save_image(name, source);
		};

		const auto t_w{ static_cast<i32>(source->width()) };
		const auto t_h{ static_cast<i32>(source->height()) };
		const auto s_w{ static_cast<i32>(target->width()) };
		const auto s_h{ static_cast<i32>(target->height()) };
		const rect<u32> src_rect{ 10, 10, s_w - 20, s_h - 20 };

		copy_test(p{           0_i32,           0_i32 }, {}, overlap::discard_target);                    // left upper corner
		copy_test(p{       t_w - s_w,           0_i32 }, {}, overlap::discard_target);                    // right upper corner
		copy_test(p{       t_w - s_w,       t_h - s_h }, {}, overlap::discard_target);                    // right lower corner
		copy_test(p{           0_i32,       t_h - s_h }, {}, overlap::discard_target);                    // left lower corner
		copy_test(p{ (t_w - s_w) / 2, (t_h - s_h) / 2 }, {}, overlap::discard_target, false, true);  // center

		source = res_man::load_image(src_image_path);
		copy_test(p{           0_i32,           0_i32 }, src_rect, overlap::discard_target);                    // left upper corner
		copy_test(p{       t_w - s_w,           0_i32 }, src_rect, overlap::discard_target);                    // right upper corner
		copy_test(p{       t_w - s_w,       t_h - s_h }, src_rect, overlap::discard_target);                    // right lower corner
		copy_test(p{           0_i32,       t_h - s_h }, src_rect, overlap::discard_target);                    // left lower corner
		copy_test(p{ (t_w - s_w) / 2, (t_h - s_h) / 2 }, src_rect, overlap::discard_target, false, true);  // center

		source = res_man::load_image(src_image_path);
		copy_test(p{           0_i32,           0_i32 }, {}, overlap::expand_source);                    // left upper corner
		copy_test(p{       t_w - s_w,           0_i32 }, {}, overlap::expand_source);                    // right upper corner
		copy_test(p{       t_w - s_w,       t_h - s_h }, {}, overlap::expand_source);                    // right lower corner
		copy_test(p{           0_i32,       t_h - s_h }, {}, overlap::expand_source);                    // left lower corner
		copy_test(p{ (t_w - s_w) / 2, (t_h - s_h) / 2 }, {}, overlap::expand_source, false, true);  // center

		source = res_man::load_image(src_image_path);
		copy_test(p{           0_i32,           0_i32 }, src_rect, overlap::expand_source);                    // left upper corner
		copy_test(p{       t_w - s_w,           0_i32 }, src_rect, overlap::expand_source);                    // right upper corner
		copy_test(p{       t_w - s_w,       t_h - s_h }, src_rect, overlap::expand_source);                    // right lower corner
		copy_test(p{           0_i32,       t_h - s_h }, src_rect, overlap::expand_source);                    // left lower corner
		copy_test(p{ (t_w - s_w) / 2, (t_h - s_h) / 2 }, src_rect, overlap::expand_source, false, true);  // center

		source = res_man::load_image(src_image_path);
		copy_test(p{    -(s_w / 2),    -(s_h / 2) }, {}, overlap::discard_target);       // left upper corner
		copy_test(p{ t_w - s_w / 2,    -(s_h / 2) }, {}, overlap::discard_target);       // right upper corner
		copy_test(p{ t_w - s_w / 2, t_h - s_h / 2 }, {}, overlap::discard_target);       // right lower corner
		copy_test(p{    -(s_w / 2), t_h - s_h / 2 }, {}, overlap::discard_target, false, true); // left lower corner

		source = res_man::load_image(src_image_path);
		copy_test(p{    -(s_w / 2),    -(s_h / 2) }, src_rect, overlap::discard_target);       // left upper corner
		copy_test(p{ t_w - s_w / 2,    -(s_h / 2) }, src_rect, overlap::discard_target);       // right upper corner
		copy_test(p{ t_w - s_w / 2, t_h - s_h / 2 }, src_rect, overlap::discard_target);       // right lower corner
		copy_test(p{    -(s_w / 2), t_h - s_h / 2 }, src_rect, overlap::discard_target, false, true); // left lower corner

		source = res_man::load_image(src_image_path);
		copy_test(p{ t_w - s_w / 2, t_h - s_h / 2 }, {}, overlap::expand_source);       // right lower corner
		copy_test(p{    -(s_w / 2),    -(s_h / 2) }, {}, overlap::expand_source, false, true);       // left upper corner

		source = res_man::load_image(src_image_path);
		copy_test(p{ t_w - s_w / 2, t_h - s_h / 2 }, src_rect, overlap::expand_source);       // right lower corner
		copy_test(p{    -(s_w / 2),    -(s_h / 2) }, src_rect, overlap::expand_source, false, true);       // left upper corner

		source = res_man::load_image(src_image_path);
		copy_test(p{ t_w - s_w / 2,    -(s_h / 2) }, {}, overlap::expand_source);       // right upper corner
		copy_test(p{    -(s_w / 2),           t_h }, {}, overlap::expand_source, false, true); // left lower corner

		source = res_man::load_image(src_image_path);
		copy_test(p{ t_w - s_w / 2,    -(s_h / 2) }, src_rect, overlap::expand_source);       // right upper corner
		copy_test(p{    -(s_w / 2),           t_h }, src_rect, overlap::expand_source, false, true); // left lower corner
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
