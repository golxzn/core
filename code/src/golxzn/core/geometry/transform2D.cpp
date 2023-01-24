#include <cmath>

#include "golxzn/core/geometry/transform2D.hpp"

namespace golxzn::core::geometry {

vec2f32 Transform2D::size() const noexcept {
	return vec2f32{ mMatrix(0, 0), mMatrix(1, 1) };
}
f32 Transform2D::area() const noexcept {
	return  mMatrix(0, 0) * mMatrix(1, 1);
}
vec2f32 Transform2D::position() const noexcept {
	return vec2f32{ mMatrix(0, 2), mMatrix(1, 2) };
}
const vec2f32 &Transform2D::pivot() const noexcept {
	return mPivot;
}

const mat3f32 &Transform2D::getMatrix() const noexcept {
	return mMatrix;
}

mat3f32 &&Transform2D::matrix() noexcept {
	return std::move(mMatrix);
}

Transform2D &Transform2D::setPivot(const vec2f32 &pivot) {
	mPivot = pivot;
	return *this;
}

Transform2D &Transform2D::translate(const f32 dx, const f32 dy) {
	mMatrix(0, 2) += dx;
	mMatrix(1, 2) += dy;
	return *this;
}

Transform2D &Transform2D::translate(const vec2f32 &offset) {
	return translate(offset.at(0), offset.at(1));
}

Transform2D &Transform2D::scale(const f32 xScale, const f32 yScale) {
	if (mPivot == DefaultPivot) {
		mMatrix(0, 0) *= xScale;
		mMatrix(1, 1) *= yScale;
		return *this;
	}
	const f32 x{ mMatrix(0, 2) };
	const f32 y{ mMatrix(1, 2) };
	mMatrix(0, 0) *= xScale;
	mMatrix(1, 1) *= yScale;
	return translate(
		(x - x * xScale) * (vec2f32::one - mPivot.at(0)),
		(y - y * yScale) * (vec2f32::one - mPivot.at(1))
	);
}
Transform2D &Transform2D::scale(const vec2f32 &_scale) {
	return scale(_scale.at(0), _scale.at(1));
}
Transform2D &Transform2D::scale(const f32 _scale) {
	return scale(_scale, _scale);
}

Transform2D &Transform2D::shear(const math::anglef32 phi, const math::anglef32 psi) {
	mMatrix(0, 1) = phi.tan();
	mMatrix(1, 0) = psi.tan();
	return *this;
}

Transform2D &Transform2D::rotate(const math::anglef32 degrees) {
	translate(-mPivot);
	const f32 sin{ degrees.sin() };
	const f32 cos{ degrees.cos() };
	const f32 y{ mMatrix(1, 2) };
	mMatrix(0, 0) *= cos; mMatrix(0, 1) *= -sin; mMatrix(0, 2) *= 1 - cos + y * sin;
	mMatrix(1, 0) *= sin; mMatrix(1, 1) *= cos; mMatrix(1, 2) = y * (1 - cos) - sin;
	return translate(mPivot);
}
Transform2D &Transform2D::reset() {
	mMatrix = mat3f32::identity();
	mPivot = DefaultPivot;
	return *this;
}


} // namespace golxzn::core::geometry
