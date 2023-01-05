#pragma once

#include <golxzn/core/matrix.hpp>
#include <golxzn/core/point.hpp>

namespace golxzn::core {

class Transform3D {
public:
	Transform3D() = default;
	explicit Transform3D(mat3r &&value) noexcept : mMatrix{ std::move(value) } {}

	void translate(real dx, real dy);
	void scale(real xScale, real yScale);
	void scale(real scale);
	void shift(real xShift, real yShift);
	void rotate(real degrees);
	void reset();

private:
	mat3r mMatrix;
};

} // namespace golxzn::core
