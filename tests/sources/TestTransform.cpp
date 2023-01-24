#include <catch2/catch_test_macros.hpp>
#include <golxzn/core/geometry/transform2D.hpp>
#include <golxzn/core/geometry/transform3D.hpp>

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

		// TODO: Add tests for shear() and rotate()
		transform.shear(30_deg, 30_deg);
		transform.rotate(45_deg);
	}
	// TODO: Add tests for methods and setPivot()
}

TEST_CASE("Transform3D", "[geometry]") {
	using namespace golxzn::core;
}