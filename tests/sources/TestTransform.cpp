#include <catch2/catch_test_macros.hpp>
#include <golxzn/core/math/vector.hpp>
#include <golxzn/core/geometry/transform2D.hpp>
#include <golxzn/core/geometry/transform3D.hpp>

#include <iostream>
#include <sstream>

TEST_CASE("Transform2D", "[geometry]") {
	using namespace golxzn::core;
	using namespace golxzn::types_literals;

	SECTION("Constructors") {
		const geometry::Transform2D defaultConstructed;

		REQUIRE(defaultConstructed.size() == vec2f32{ 1.0_f32, 1.0_f32 });
		REQUIRE(defaultConstructed.area() == 1.0_f32);
		REQUIRE(defaultConstructed.position() == vec2f32{ 0.0_f32, 0.0_f32 });
		REQUIRE(defaultConstructed.pivot() == geometry::Transform2D::DefaultPivot);

		const geometry::Transform2D moveConstructed{ mat3f32{
			5.0_f32, 0.0_f32, 3.0_f32,
			0.0_f32, 5.0_f32, 4.0_f32,
			0.0_f32, 0.0_f32, 1.0_f32
		} };

		REQUIRE(moveConstructed.size() == vec2f32{ 5.0_f32, 5.0_f32 });
		REQUIRE(moveConstructed.area() == 25.0_f32);
		REQUIRE(moveConstructed.position() == vec2f32{ 3.0_f32, 4.0_f32 });
		REQUIRE(moveConstructed.pivot() == geometry::Transform2D::DefaultPivot);
	}

	SECTION("Methods") {
		using namespace golxzn::angle_literals;
		geometry::Transform2D transform{};

		transform.translate(2.0_f32, 2.0_f32);
		REQUIRE(transform.position() == vec2f32{ 2.0_f32, 2.0_f32 });
		transform.scale(3.0_f32, 3.0_f32);
		REQUIRE(transform.size() == vec2f32{ 3.0_f32, 3.0_f32 });
		REQUIRE((transform * vec2f32{ 1.0_f32, 1.0_f32 }) == vec2f32{ 5.0_f32, 5.0_f32 });
		transform.rotate(45_deg);
		const auto rotated{ transform * vec2f32{ 1.0_f32, 1.0_f32 } };
		std::stringstream info;
		info << rotated << " != " << vec2f32{ 0.0_f32, 7.071_f32 };
		INFO(info.str());
		REQUIRE(rotated == vec2f32{ 0.0_f32, 7.071_f32 });
		transform.shear(30_deg, 30_deg);
		REQUIRE((transform * vec2f32{ 1.0_f32, 1.0_f32 }) == vec2f32{ 4.949_f32, 4.949_f32 });
	}
	// TODO: Add tests for methods and setPivot()
}

TEST_CASE("Transform3D", "[geometry]") {
	using namespace golxzn::core;
	using namespace golxzn::types_literals;

	SECTION("Constructors") {
		const geometry::Transform3D defaultConstructed;

		REQUIRE(defaultConstructed.size() == vec3f32{ 1.0_f32, 1.0_f32, 1.0_f32 });
		REQUIRE(defaultConstructed.volume() == 1.0_f32);
		REQUIRE(defaultConstructed.position() == vec3f32{ 0.0_f32, 0.0_f32, 0.0_f32 });
		REQUIRE(defaultConstructed.pivot() == geometry::Transform3D::DefaultPivot);

		const geometry::Transform3D moveConstructed{ mat4f32{
			5.0_f32, 0.0_f32, 0.0_f32, 3.0_f32,
			0.0_f32, 5.0_f32, 0.0_f32, 4.0_f32,
			0.0_f32, 0.0_f32, 5.0_f32, 5.0_f32,
			0.0_f32, 0.0_f32, 0.0_f32, 1.0_f32
		} };

		REQUIRE(moveConstructed.size() == vec3f32{ 5.0_f32, 5.0_f32, 5.0_f32 });
		REQUIRE(moveConstructed.volume() == 125.0_f32);
		REQUIRE(moveConstructed.position() == vec3f32{ 3.0_f32, 4.0_f32, 5.0_f32 });
		REQUIRE(moveConstructed.pivot() == geometry::Transform3D::DefaultPivot);
	}

	SECTION("Methods") {
		using namespace golxzn::angle_literals;
		geometry::Transform3D transform{};

		transform.translate(2.0_f32, 2.0_f32, 2.0_f32);
		REQUIRE(transform.position() == vec3f32{ 2.0_f32, 2.0_f32, 2.0_f32 });
		transform.scale(3.0_f32, 3.0_f32, 3.0_f32);
		REQUIRE(transform.size() == vec3f32{ 3.0_f32, 3.0_f32, 3.0_f32 });

		// TODO: Add tests for shear() and rotate()
		transform.shear(30_deg, 30_deg, 30_deg);
		transform.rotate(45_deg, get_axis<f32, 3, axis::Y>());
	}
}