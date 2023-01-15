#pragma once

#include <golxzn/core/math/matrix.hpp>
#include <golxzn/core/math/point.hpp>

namespace golxzn::core {

class Transform3D {
public:
	Transform3D() = default;
	explicit Transform3D(mat3f64 &&value) noexcept : mMatrix{ std::move(value) } {}

	void translate(f32 dx, f32 dy);
	void scale(f32 xScale, f32 yScale);
	void scale(f32 scale);
	void shift(f32 xShift, f32 yShift);
	void rotate(f32 degrees);
	void reset();

private:
	mat3f64 mMatrix;
};

} // namespace golxzn::core
